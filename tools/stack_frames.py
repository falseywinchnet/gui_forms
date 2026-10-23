#!/usr/bin/env python3
"""Find first-party functions whose stack frame is larger than a limit.

A sanitizer build makes stack frames much larger: AddressSanitizer puts a
redzone around every local whose address escapes. A test main or fixture
holding many locals can then need a frame of hundreds of kilobytes, and a
chain of such frames overflows the main thread's stack (1 MB on Windows,
8 MB on Linux and macOS, 512 KB on a macOS secondary thread). `check` reads
the prologue of every function in a build tree's first-party objects and
fails on any frame over the limit, naming the function and its object.

How a frame is read. The disassembler (llvm-objdump, else GNU objdump) lists
each function's instructions. The first MAX_PROLOGUE of them are followed,
with every register that holds a stack address and its depth below the
stack pointer at entry. The frame is the deepest such address. The prologue
ends at a return, an unconditional jump, or a call that is not a stack
probe. This covers the forms LLVM and GCC emit:

- x86-64, all three object formats: `push`, `sub $N, %rsp`, and the probed
  frame `mov $N, %eax; call ___chkstk_ms; sub %rax, %rsp` (Windows; also
  `___chkstk_darwin`);
- x86-64 stack clash protection: `mov %rsp, %r11; sub $N, %r11`, then a
  probe loop ending `cmp %r11, %rsp`;
- AArch64 (Linux and macOS): `sub sp, sp, #N, lsl #12` pairs, a scratch
  register (`sub x9, sp, #N, lsl #12; and sp, x9, #mask`) when the frame is
  realigned, pre-indexed stores (`stp x29, x30, [sp, #-N]!`), and the same
  probe loop (`cmp sp, x9`).

A frame grown later by alloca or a variable-length array is not seen. The
pushes and stores of saved registers are counted, so the figure is within
a few hundred bytes of the frame the function allocates. AddressSanitizer
may move a frame of at most 64 KB to its fake stack; such a function calls
`__asan_stack_malloc_N` first and allocates its stack fallback after the
call, so it is read short, but it is far below any limit this check sets.
Larger frames are allocated in the prologue and read in full.

Where it runs: the Linux x64 sanitizer job in CI, and any local sanitizer
tree (Windows x64 trees through llvm-objdump from the pinned toolchain).
The AArch64 forms are checked against listings of cross-compiled objects
for Linux and macOS; no CI job builds an AArch64 sanitizer tree yet.

First-party objects are the build tree's *.o and *.obj files inside a CMake
target directory (`<target>.dir`) and outside third_party/. Run it after a
build: a narrowed build (ci_build.py --changed-from or --tests) leaves only
the objects it built, and an incremental tree may still hold objects of
removed sources.

The exit status is 0 when no frame is over the limit, 1 when one is, and 2
when the tree holds no first-party object or the disassembler fails.
"""
from __future__ import annotations
import argparse
from concurrent.futures import ProcessPoolExecutor
from itertools import repeat
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

DEFAULT_LIMIT: int = 256 * 1024
# Instructions read from each function's start: enough for 8 pushes, a
# probed or two-part frame, a probe loop and the sanitizer's first stores.
MAX_PROLOGUE: int = 64
REPORT_FROM: int = 64 * 1024
# Objects per disassembler run.
BATCH: int = 16
DISASSEMBLY: list[str] = ['-d', '-r', '-C', '--no-show-raw-insn']

LABEL: re.Pattern[str] = re.compile(r'^([0-9a-f]+) <(.+)>:$')
SECTION: re.Pattern[str] = re.compile(r'^Disassembly of section (.+):$')
FILE_FORMAT: re.Pattern[str] = re.compile(r'file format (\S+(?: \S+)*)\s*$')
# An address, then a relocation type: upper case, unlike every mnemonic.
RELOCATION: re.Pattern[str] = re.compile(r'^\s+[0-9a-f]+:\s+([A-Z][A-Z0-9_]*)\s+(\S+)')
INSTRUCTION: re.Pattern[str] = re.compile(r'^\s+[0-9a-f]+:\s+(\S+)\s*(.*)$')
SYMBOL: re.Pattern[str] = re.compile(r'^([0-9a-f]+)\s(.{7})\s(\S+)\s+(.+)$')
PROBE: re.Pattern[str] = re.compile(r'chkstk|probestack|_alloca\b')
MACH_O_SECTION_LABEL: re.Pattern[str] = re.compile(r'ltmp[0-9]+')
X86_PREFIXES: frozenset[str] = frozenset(('lock', 'rep', 'repz', 'repe', 'repnz', 'repne', 'notrack', 'bnd',
                                          'data16', 'cs', 'ds', 'es', 'ss', 'fs', 'gs'))
X86_MNEMONICS: frozenset[str] = frozenset(('push', 'sub', 'add', 'mov', 'movabs', 'lea', 'and', 'cmp', 'call',
                                           'ret', 'jmp', 'test'))
X86_WIDE: dict[str, str] = {
    'eax': 'rax', 'ebx': 'rbx', 'ecx': 'rcx', 'edx': 'rdx', 'esi': 'rsi', 'edi': 'rdi', 'ebp': 'rbp', 'esp': 'sp',
    'rsp': 'sp',
}


class FrameState:
    """The registers known to hold a stack address or a constant, while one prologue is followed.

    stack maps a register to its depth in bytes below the stack pointer at
    the function's entry; `sp` is the stack pointer itself. constants maps
    a register to the integer it holds.
    """

    def __init__(self) -> None:
        self.stack: dict[str, int] = {'sp': 0}
        self.constants: dict[str, int] = {}
        self.deepest: int = 0

    def forget(self, register: str) -> None:
        self.stack.pop(register, None)
        self.constants.pop(register, None)

    def set_depth(self, register: str, depth: int) -> None:
        self.constants.pop(register, None)
        self.stack[register] = depth
        if depth > self.deepest:
            self.deepest = depth

    def set_constant(self, register: str, value: int) -> None:
        self.stack.pop(register, None)
        self.constants[register] = value

    def equalize(self, first: str, second: str) -> None:
        """A probe loop compares two stack registers and runs until they are equal."""
        if first in self.stack and second in self.stack:
            depth: int = max(self.stack[first], self.stack[second])
            self.set_depth(first, depth)
            self.set_depth(second, depth)


class Instruction:
    """One disassembled instruction: mnemonic, operands, and the symbol a relocation on it names."""

    def __init__(self, mnemonic: str, operands: list[str]) -> None:
        self.mnemonic: str = mnemonic
        self.operands: list[str] = operands
        self.relocation: str = ''


class Function:
    def __init__(self, name: str, section: str, address: int) -> None:
        self.name: str = name
        self.section: str = section
        self.address: int = address
        self.instructions: list[Instruction] = []


def split_operands(text: str) -> list[str]:
    """Operands separated by commas outside brackets and parentheses, stripped."""
    operands: list[str] = []
    depth: int = 0
    start: int = 0
    index: int
    character: str
    for index, character in enumerate(text):
        if character in '([':
            depth += 1
        elif character in ')]':
            depth -= 1
        elif character == ',' and depth == 0:
            operands.append(text[start:index].strip())
            start = index + 1
    last: str = text[start:].strip()
    if last:
        operands.append(last)
    return operands


def strip_comment(text: str, architecture: str) -> str:
    """The operand text without the disassembler's comment.

    x86 comments start with `#` (an AT&T operand never holds one). AArch64
    immediates start with `#`, so its comments are `//` (ELF, COFF) or `;`
    (Mach-O).
    """
    markers: tuple[str, ...] = ('#',) if architecture == 'x86_64' else ('//', ';')
    end: int = len(text)
    marker: str
    for marker in markers:
        found: int = text.find(marker)
        if found >= 0 and found < end:
            end = found
    return text[:end].strip()


def architecture_of(file_format: str) -> str:
    """x86_64 or aarch64 from the disassembler's `file format` text; '' for any other."""
    lowered: str = file_format.lower()
    if 'aarch64' in lowered or 'arm64' in lowered:
        return 'aarch64'
    if 'x86-64' in lowered or 'x86_64' in lowered:
        return 'x86_64'
    return ''


def parse_listing(text: str) -> tuple[str, list[Function]]:
    """The architecture and the functions of one `objdump -d -r` listing, in listing order."""
    architecture: str = ''
    section: str = ''
    functions: list[Function] = []
    current: Function | None = None
    line: str
    for line in text.splitlines():
        if not line:
            continue
        if not line[0].isspace():
            # A file format, section or function label line.
            if not architecture:
                format_match: re.Match[str] | None = FILE_FORMAT.search(line)
                if format_match is not None:
                    architecture = architecture_of(format_match.group(1))
                    continue
            section_match: re.Match[str] | None = SECTION.match(line)
            if section_match is not None:
                section = section_match.group(1)
                current = None
                continue
            label: re.Match[str] | None = LABEL.match(line)
            if label is not None:
                current = Function(label.group(2), section, int(label.group(1), 16))
                functions.append(current)
            continue
        # Instructions and relocations are indented. Past the prologue (one
        # instruction more, so its relocation is kept) they are skipped
        # unparsed: most of a listing is function bodies.
        if current is None or len(current.instructions) > MAX_PROLOGUE:
            continue
        relocation: re.Match[str] | None = RELOCATION.match(line)
        if relocation is not None:
            if current.instructions:
                current.instructions[-1].relocation = relocation.group(2)
            continue
        instruction: re.Match[str] | None = INSTRUCTION.match(line)
        if instruction is None:
            continue
        tokens: list[str] = (instruction.group(1) + ' ' + instruction.group(2)).split(None, 1)
        mnemonic: str = tokens[0]
        rest: str = tokens[1] if len(tokens) > 1 else ''
        while architecture == 'x86_64' and (mnemonic in X86_PREFIXES or mnemonic.startswith('rex')) and rest:
            tokens = rest.split(None, 1)
            mnemonic = tokens[0]
            rest = tokens[1] if len(tokens) > 1 else ''
        current.instructions.append(Instruction(mnemonic, split_operands(strip_comment(rest, architecture))))
    return architecture, functions


def immediate(operand: str) -> int | None:
    """The value of an immediate operand (`$0x10`, `#-0x50`, `#16`), or None."""
    if not operand or operand[0] not in '$#':
        return None
    try:
        return int(operand[1:], 0)
    except ValueError:
        return None


def x86_register(operand: str) -> str | None:
    """The 64-bit name of a general register operand (`%eax` gives rax, `%rsp` gives sp), or None."""
    if not operand.startswith('%'):
        return None
    name: str = operand[1:]
    if name in X86_WIDE:
        return X86_WIDE[name]
    if name.startswith('r') and name.endswith('d') and name[1:-1].isdigit():
        return name[:-1]
    return name


def x86_mnemonic(mnemonic: str) -> str:
    """The mnemonic without its AT&T size suffix: subq gives sub, movabsq gives mov, callq gives call."""
    name: str = mnemonic
    if name not in X86_MNEMONICS and name[-1:] in ('q', 'l', 'w', 'b') and name[:-1] in X86_MNEMONICS:
        name = name[:-1]
    if name == 'movabs':
        name = 'mov'
    return name


def is_probe_call(instruction: Instruction) -> bool:
    """Whether a call goes to a stack probe, named by its relocation or by its resolved target."""
    if PROBE.search(instruction.relocation):
        return True
    operand: str
    for operand in instruction.operands:
        if PROBE.search(operand):
            return True
    return False


def x86_lea_base(operand: str) -> tuple[int, str] | None:
    """The displacement and base register of `disp(%base)` with no index, or None."""
    match: re.Match[str] | None = re.fullmatch(r'(-?(?:0x[0-9a-f]+|[0-9]+))?\((%\w+)\)', operand)
    if match is None:
        return None
    displacement: int = int(match.group(1), 0) if match.group(1) else 0
    base: str | None = x86_register(match.group(2))
    if base is None:
        return None
    return displacement, base


def follow_x86(instruction: Instruction, state: FrameState) -> bool:
    """Apply one x86-64 (AT&T) instruction to the state; False when the prologue has ended."""
    mnemonic: str = x86_mnemonic(instruction.mnemonic)
    operands: list[str] = instruction.operands
    if mnemonic in ('ret', 'jmp', 'ud2', 'hlt'):
        return False
    if mnemonic == 'call':
        return is_probe_call(instruction)
    if mnemonic == 'push':
        state.set_depth('sp', state.stack['sp'] + 8)
        return True
    if len(operands) != 2:
        return True
    source: str = operands[0]
    target: str | None = x86_register(operands[1])
    if target is None:
        return True
    value: int | None = immediate(source)
    source_register: str | None = x86_register(source)
    if mnemonic == 'cmp':
        if source_register is not None:
            state.equalize(source_register, target)
        return True
    if mnemonic == 'test':
        return True
    if mnemonic == 'sub' and target in state.stack:
        if value is not None:
            state.set_depth(target, state.stack[target] + value)
        elif source_register is not None and source_register in state.constants:
            state.set_depth(target, state.stack[target] + state.constants[source_register])
        else:
            state.forget(target)
        return True
    if mnemonic == 'add' and target in state.stack and value is not None:
        state.set_depth(target, state.stack[target] - value)
        return True
    if mnemonic == 'and' and target in state.stack:
        return True
    if mnemonic == 'mov':
        if value is not None:
            state.set_constant(target, value)
        elif source_register is not None and source_register in state.stack:
            state.set_depth(target, state.stack[source_register])
        elif source_register is not None and source_register in state.constants:
            state.set_constant(target, state.constants[source_register])
        else:
            state.forget(target)
        return True
    if mnemonic == 'lea':
        base: tuple[int, str] | None = x86_lea_base(source)
        if base is not None and base[1] in state.stack:
            state.set_depth(target, state.stack[base[1]] - base[0])
        else:
            state.forget(target)
        return True
    if target != 'sp':
        state.forget(target)
    return True


def aarch64_register(operand: str) -> str | None:
    """The 64-bit name of a general register operand (`w9` gives x9, `wsp` gives sp), or None."""
    if operand in ('sp', 'wsp'):
        return 'sp'
    if re.fullmatch(r'[xw][0-9]+', operand):
        return 'x' + operand[1:]
    if operand in ('fp', 'x29'):
        return 'x29'
    if operand in ('lr', 'x30'):
        return 'x30'
    return None


def aarch64_shift(operands: list[str], index: int) -> int:
    """The shift operands[index] names (`lsl #12`, `uxtx #4`), 0 when there is none."""
    if len(operands) <= index:
        return 0
    parts: list[str] = operands[index].split()
    if len(parts) == 2 and parts[0] in ('lsl', 'uxtx', 'sxtx', 'uxtw', 'sxtw'):
        amount: int | None = immediate(parts[1])
        if amount is not None:
            return amount
    return 0


def aarch64_writeback(operands: list[str]) -> int:
    """The bytes a pre-indexed store to sp (`[sp, #-0x50]!`) moves the stack pointer down."""
    if not operands or not operands[-1].endswith('!'):
        return 0
    match: re.Match[str] | None = re.fullmatch(r'\[sp,\s*#(-?(?:0x[0-9a-f]+|[0-9]+))\]!', operands[-1])
    if match is None:
        return 0
    return -int(match.group(1), 0)


def follow_aarch64(instruction: Instruction, state: FrameState) -> bool:
    """Apply one AArch64 instruction to the state; False when the prologue has ended."""
    mnemonic: str = instruction.mnemonic
    operands: list[str] = instruction.operands
    if mnemonic in ('ret', 'b', 'br', 'blr', 'brk', 'udf'):
        return False
    if mnemonic == 'bl':
        return is_probe_call(instruction)
    if mnemonic.startswith('st') and mnemonic not in ('stlr',):
        moved: int = aarch64_writeback(operands)
        if moved:
            state.set_depth('sp', state.stack['sp'] + moved)
        return True
    if not operands:
        return True
    target: str | None = aarch64_register(operands[0])
    if mnemonic == 'cmp' and len(operands) >= 2:
        second: str | None = aarch64_register(operands[1])
        if target is not None and second is not None:
            state.equalize(target, second)
        return True
    if target is None or mnemonic.startswith('b.') or mnemonic in ('cbz', 'cbnz', 'tbz', 'tbnz'):
        return True
    if mnemonic in ('sub', 'add') and len(operands) >= 3:
        source: str | None = aarch64_register(operands[1])
        value: int | None = immediate(operands[2])
        amount: int | None = None
        if value is not None:
            amount = value << aarch64_shift(operands, 3)
        else:
            register: str | None = aarch64_register(operands[2])
            if register is not None and register in state.constants:
                amount = state.constants[register] << aarch64_shift(operands, 3)
        if source is not None and source in state.stack and amount is not None:
            if mnemonic == 'sub':
                state.set_depth(target, state.stack[source] + amount)
            else:
                state.set_depth(target, state.stack[source] - amount)
        elif target != 'sp':
            state.forget(target)
        return True
    if mnemonic == 'and' and len(operands) >= 2:
        source = aarch64_register(operands[1])
        if source is not None and source in state.stack:
            state.set_depth(target, state.stack[source])
        elif target != 'sp':
            state.forget(target)
        return True
    if mnemonic in ('mov', 'movz') and len(operands) >= 2:
        value = immediate(operands[1])
        source = aarch64_register(operands[1])
        if value is not None:
            state.set_constant(target, value << aarch64_shift(operands, 2))
        elif source is not None and source in state.stack:
            state.set_depth(target, state.stack[source])
        elif source is not None and source in state.constants:
            state.set_constant(target, state.constants[source])
        else:
            state.forget(target)
        return True
    if mnemonic == 'movk' and len(operands) >= 2 and target in state.constants:
        value = immediate(operands[1])
        shift: int = aarch64_shift(operands, 2)
        if value is not None:
            kept: int = state.constants[target] & ~(0xffff << shift)
            state.set_constant(target, kept | (value << shift))
        return True
    if target != 'sp':
        state.forget(target)
    return True


def frame_size(architecture: str, function: Function) -> int:
    """The deepest stack address the function's prologue reaches, in bytes below its entry."""
    state: FrameState = FrameState()
    instruction: Instruction
    for instruction in function.instructions[:MAX_PROLOGUE]:
        going: bool
        if architecture == 'x86_64':
            going = follow_x86(instruction, state)
        else:
            going = follow_aarch64(instruction, state)
        if not going:
            break
    return state.deepest


def parse_symbols(text: str) -> dict[tuple[str, int], str]:
    """Function symbols of an `objdump -t` listing by (section, address), skipping assembler temporaries."""
    names: dict[tuple[str, int], str] = {}
    line: str
    for line in text.splitlines():
        match: re.Match[str] | None = SYMBOL.match(line)
        if match is None or 'F' not in match.group(2):
            continue
        name: str = match.group(4).strip()
        if MACH_O_SECTION_LABEL.fullmatch(name) or name.startswith('l_') or name.startswith('L'):
            continue
        names[(match.group(3), int(match.group(1), 16))] = name
    return names


class ObjectFrames:
    """One object's result: its path, functions with their frames, and the disassembler's error."""

    def __init__(self, path: Path) -> None:
        self.path: Path = path
        self.frames: list[tuple[str, int]] = []
        self.error: str = ''


def run_disassembler(objdump: str, arguments: list[str], paths: list[Path]) -> subprocess.CompletedProcess[str]:
    command: list[str] = [objdump]
    command.extend(arguments)
    path: Path
    for path in paths:
        command.append(str(path))
    return subprocess.run(command, capture_output=True, text=True, errors='replace', check=False)


def split_listing(text: str) -> list[str]:
    """One listing of several objects, cut at each object's `file format` line, in order."""
    listings: list[list[str]] = []
    line: str
    for line in text.splitlines():
        if line and not line[0].isspace() and FILE_FORMAT.search(line) is not None:
            listings.append([])
        if listings:
            listings[-1].append(line)
    joined: list[str] = []
    lines: list[str]
    for lines in listings:
        joined.append('\n'.join(lines))
    return joined


def measure_listing(objdump: str, path: Path, listing: str) -> ObjectFrames:
    """Every function's frame in one object's listing."""
    result: ObjectFrames = ObjectFrames(path)
    architecture: str
    functions: list[Function]
    architecture, functions = parse_listing(listing)
    if not architecture:
        result.error = 'not an x86-64 or AArch64 object'
        return result
    symbols: dict[tuple[str, int], str] = {}
    function: Function
    for function in functions:
        if MACH_O_SECTION_LABEL.fullmatch(function.name):
            # Mach-O: llvm-objdump labels a section's first function with the
            # section's temporary symbol; the symbol table names it.
            table: subprocess.CompletedProcess[str] = run_disassembler(objdump, ['-t', '-C'], [path])
            symbols = parse_symbols(table.stdout)
            break
    for function in functions:
        if function.name == function.section:
            continue
        name: str = function.name
        if MACH_O_SECTION_LABEL.fullmatch(name):
            name = symbols.get((function.section, function.address), name)
        result.frames.append((name, frame_size(architecture, function)))
    return result


def batch_frames(objdump: str, paths: list[Path]) -> list[ObjectFrames]:
    """Disassemble a batch of objects in one run and measure every function's frame.

    Objects are read BATCH at a time to spare a disassembler start per
    object. Batches run in worker processes: reading the listings is
    Python work, which threads would serialize. When a run fails, each
    object of its batch is read alone, so the failure is named by its
    object.
    """
    listing: subprocess.CompletedProcess[str] = run_disassembler(objdump, DISASSEMBLY, paths)
    listings: list[str] = split_listing(listing.stdout)
    results: list[ObjectFrames] = []
    path: Path
    if listing.returncode != 0 or len(listings) != len(paths):
        if len(paths) > 1:
            for path in paths:
                results.extend(batch_frames(objdump, [path]))
            return results
        failed: ObjectFrames = ObjectFrames(paths[0])
        failed.error = listing.stderr.strip() or f'exit status {listing.returncode}'
        results.append(failed)
        return results
    index: int
    for index, path in enumerate(paths):
        if len(paths) > 1 and '<unknown>' in listings[index]:
            # llvm-objdump decodes a batch with the first object's target, so
            # an object of another architecture is read alone.
            results.extend(batch_frames(objdump, [path]))
        else:
            results.append(measure_listing(objdump, path, listings[index]))
    return results


def first_party_objects(tree: Path) -> list[Path]:
    """The tree's object files that belong to a CMake target and not to third_party/, sorted."""
    found: list[Path] = []
    folder: str
    names: list[str]
    for folder, _, names in os.walk(tree):
        relative: Path = Path(folder).relative_to(tree)
        parts: tuple[str, ...] = relative.parts
        if 'third_party' in parts or 'CMakeScratch' in parts or 'CMakeTmp' in parts:
            continue
        in_target: bool = False
        part: str
        for part in parts:
            if part.endswith('.dir'):
                in_target = True
        if not in_target:
            continue
        name: str
        for name in names:
            if name.endswith('.o') or name.endswith('.obj'):
                found.append(Path(folder) / name)
    found.sort()
    return found


def find_disassembler(requested: str) -> str | None:
    """The disassembler to run: the requested one, else llvm-objdump, else objdump."""
    if requested:
        return requested
    candidate: str
    for candidate in ('llvm-objdump', 'objdump'):
        path: str | None = shutil.which(candidate)
        if path is not None:
            return path
    return None


def main(argv: list[str] | None = None) -> int:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('command', choices=['check'])
    parser.add_argument('--tree', type=Path, required=True, help='a CMake build tree')
    parser.add_argument('--limit', type=int, default=DEFAULT_LIMIT, help='largest frame admitted, in bytes')
    parser.add_argument('--objdump', default='', help='disassembler (default: llvm-objdump, else objdump)')
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    parser.add_argument('--top', type=int, default=10, help='how many of the deepest frames to print')
    parser.add_argument('--report', type=Path, help=f'write every frame of at least {REPORT_FROM} bytes as TSV')
    args: argparse.Namespace = parser.parse_args(argv)
    objdump: str | None = find_disassembler(args.objdump)
    if objdump is None:
        print('stack frames: no llvm-objdump or objdump on PATH', file=sys.stderr)
        return 2
    objects: list[Path] = first_party_objects(args.tree)
    if not objects:
        print(f'stack frames: no first-party object under {args.tree}', file=sys.stderr)
        return 2
    batches: list[list[Path]] = []
    start: int
    for start in range(0, len(objects), BATCH):
        batches.append(objects[start:start + BATCH])
    results: list[ObjectFrames] = []
    with ProcessPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        batch: list[ObjectFrames]
        for batch in pool.map(batch_frames, repeat(objdump), batches):
            results.extend(batch)
    rows: list[tuple[int, str, str]] = []
    errors: list[str] = []
    function_count: int = 0
    result: ObjectFrames
    for result in results:
        relative: str = result.path.relative_to(args.tree).as_posix()
        if result.error:
            errors.append(relative + ': ' + result.error)
            continue
        name: str
        size: int
        for name, size in result.frames:
            function_count += 1
            rows.append((size, name, relative))
    rows.sort(reverse=True)
    print(f'stack frames: {len(objects)} first-party objects, {function_count} functions, '
          f'limit {args.limit} bytes ({objdump})')
    row: tuple[int, str, str]
    for row in rows[:args.top]:
        print(f'  {row[0]:>9}  {row[1]}  [{row[2]}]')
    if args.report is not None:
        lines: list[str] = ['frame_bytes\tfunction\tobject']
        for row in rows:
            if row[0] >= REPORT_FROM:
                lines.append(f'{row[0]}\t{row[1]}\t{row[2]}')
        args.report.write_text('\n'.join(lines) + '\n', encoding='utf-8')
    over: list[tuple[int, str, str]] = []
    for row in rows:
        if row[0] > args.limit:
            over.append(row)
    if errors:
        print(f'stack frames: the disassembler failed on {len(errors)} objects:', file=sys.stderr)
        error: str
        for error in errors:
            print('  ' + error, file=sys.stderr)
        return 2
    if over:
        print(f'stack frames: {len(over)} functions have a frame over {args.limit} bytes:')
        for row in over:
            print(f'  {row[0]} bytes  {row[1]}  [{row[2]}]')
        return 1
    print(f'stack frames: none over {args.limit} bytes')
    return 0


if __name__ == '__main__':
    sys.exit(main())
