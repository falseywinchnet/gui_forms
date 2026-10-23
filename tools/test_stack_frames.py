#!/usr/bin/env python3
"""Tests for stack_frames.py: reading prologues of each architecture and object format, and choosing objects.

The listings are llvm-objdump 22 output (`-d -r -C --no-show-raw-insn`) for
functions holding a 1,100,000-byte local array, compiled with
-fsanitize=address for each target, cut to their prologues; one is GNU
objdump's syntax for the same x86-64 frame.
"""
from __future__ import annotations
from pathlib import Path
import tempfile
import unittest

import ci_build
import stack_frames

WINDOWS_X64: str = '''
probe.o:	file format coff-x86-64

Disassembly of section .text:

0000000000000000 <.text>:
       0:      	decb	%dl

0000000000000008 <main>:
       8:      	pushq	%rbp
       9:      	pushq	%r15
       b:      	pushq	%r14
       d:      	pushq	%r13
       f:      	pushq	%r12
      11:      	pushq	%rsi
      12:      	pushq	%rdi
      13:      	pushq	%rbx
      14:      	movl	$0xc21d8, %eax          # imm = 0xC21D8
      19:      	callq	0x1e <main+0x16>
		000000000000001a:  IMAGE_REL_AMD64_REL32	___chkstk_ms
      1e:      	subq	%rax, %rsp
      21:      	leaq	0x80(%rsp), %rbp
      29:      	movaps	%xmm7, 0xc2140(%rbp)
      37:      	andq	$-0x20, %rsp
'''

LINUX_X64: str = '''
probe.o:	file format elf64-x86-64

Disassembly of section .text:

0000000000000000 <big_frame>:
       0:      	pushq	%rbp
       1:      	movq	%rsp, %rbp
       4:      	pushq	%r15
       6:      	pushq	%r14
       8:      	pushq	%r13
       a:      	pushq	%r12
       c:      	pushq	%rbx
       d:      	andq	$-0x20, %rsp
      11:      	subq	$0x10ca20, %rsp         # imm = 0x10CA20
      18:      	movl	%edi, %r14d
      1b:      	leaq	0x20(%rsp), %r15
      28:      	leaq	(%rip), %rax            # 0x2f <big_frame+0x2f>
		000000000000002b:  R_X86_64_PC32	.L___asan_gen_stack-0x4
      2f:      	callq	0x34 <big_frame+0x34>
		0000000000000030:  R_X86_64_PLT32	consume-0x4
      34:      	subq	$0x7fffffff, %rsp

0000000000000100 <tiny_frame(int)>:
     100:      	pushq	%rbp
     101:      	movq	%rsp, %rbp
     104:      	subq	$0x40, %rsp
     108:      	retq
'''

LINUX_X64_GNU: str = '''
probe.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <big_frame>:
   0:	push   %rbp
   1:	mov    %rsp,%rbp
   4:	push   %r15
   6:	push   %r14
   8:	push   %r13
   a:	push   %r12
   c:	push   %rbx
   d:	and    $0xffffffffffffffe0,%rsp
  11:	sub    $0x10ca20,%rsp
  18:	mov    %edi,%r14d
			2b: R_X86_64_PC32	.L___asan_gen_stack-0x4
'''

LINUX_X64_STACK_CLASH: str = '''
probe.o:	file format elf64-x86-64

Disassembly of section .text:

0000000000000000 <big_frame>:
       0:      	movq	%rsp, %r11
       3:      	subq	$0x10c000, %r11         # imm = 0x10C000
       a:      	subq	$0x1000, %rsp           # imm = 0x1000
      11:      	movq	$0x0, (%rsp)
      19:      	cmpq	%r11, %rsp
      1c:      	jne	0xa <big_frame+0xa>
      1e:      	subq	$0x8e8, %rsp            # imm = 0x8E8
      25:      	movl	%edi, %esi
      2a:      	callq	0x2f <big_frame+0x2f>
'''

LINUX_ARM64_REALIGNED: str = '''
probe.o:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000000000 <big_frame>:
       0:      	stp	x29, x30, [sp, #-0x50]!
       4:      	str	x28, [sp, #0x10]
       8:      	stp	x24, x23, [sp, #0x20]
       c:      	stp	x22, x21, [sp, #0x30]
      10:      	stp	x20, x19, [sp, #0x40]
      14:      	mov	x29, sp
      18:      	sub	x9, sp, #0x10c, lsl #12 // =0x10c000
      1c:      	sub	x9, x9, #0xa10
      20:      	and	sp, x9, #0xffffffffffffffe0
      24:      	mov	x21, sp
      28:      	mov	w8, #0x8ab3             // =35507
'''

LINUX_ARM64_PLAIN: str = '''
probe.o:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000000000 <big_frame>:
       0:      	stp	x29, x30, [sp, #-0x20]!
       4:      	str	x28, [sp, #0x10]
       8:      	mov	x29, sp
       c:      	sub	sp, sp, #0x10c, lsl #12 // =0x10c000
      10:      	sub	sp, sp, #0x8e0
      14:      	mov	w1, w0
      18:      	mov	x0, sp
      1c:      	bl	0x1c <big_frame+0x1c>
		000000000000001c:  R_AARCH64_CALL26	consume
      20:      	sub	sp, sp, #0x7ff, lsl #12
'''

LINUX_ARM64_STACK_CLASH: str = '''
probe.o:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000000000 <big_frame>:
       0:      	stp	x29, x30, [sp, #-0x20]!
       4:      	str	x28, [sp, #0x10]
       8:      	mov	x29, sp
       c:      	sub	x9, sp, #0x10c, lsl #12 // =0x10c000
      10:      	sub	sp, sp, #0x1, lsl #12   // =0x1000
      14:      	cmp	sp, x9
      18:      	str	xzr, [sp]
      1c:      	b.ne	0x10 <big_frame+0x10>
      20:      	sub	sp, sp, #0x8e0
      24:      	mov	w1, w0
'''

MACOS_ARM64: str = '''
probe.o:	file format mach-o arm64

Disassembly of section __TEXT,__text:

0000000000000000 <ltmp0>:
       0:      	stp	x24, x23, [sp, #-0x40]!
       4:      	stp	x22, x21, [sp, #0x10]
       8:      	stp	x20, x19, [sp, #0x20]
       c:      	stp	x29, x30, [sp, #0x30]
      10:      	add	x29, sp, #0x30
      14:      	sub	x9, sp, #0x10c, lsl #12 ; =0x10c000
      18:      	sub	x9, x9, #0xa20
      1c:      	and	sp, x9, #0xffffffffffffffe0
      20:      	mov	x19, x0

000000000000012c <_middle_frame>:
     12c:      	stp	x24, x23, [sp, #-0x40]!
     130:      	sub	x9, sp, #0x49, lsl #12  ; =0x49000
     134:      	sub	x9, x9, #0x520
     138:      	and	sp, x9, #0xffffffffffffffe0
'''

MACOS_ARM64_SYMBOLS: str = '''
probe.o:	file format mach-o arm64

SYMBOL TABLE:
0000000000000000 l     F __TEXT,__text ltmp0
0000000000000524 l     O __TEXT,__cstring l____asan_gen_stack
000000000000050c l     F __TEXT,__text _asan.module_ctor
0000000000000000 g     F __TEXT,__text _big_frame
000000000000012c g     F __TEXT,__text _middle_frame
0000000000000000         *UND* ___asan_init
'''

MOVK_CONSTANT: str = '''
probe.o:	file format elf64-littleaarch64

Disassembly of section .text:

0000000000000000 <probed>:
       0:      	stp	x29, x30, [sp, #-0x10]!
       4:      	mov	x15, #0x8000
       8:      	movk	x15, #0x4, lsl #16
       c:      	sub	sp, sp, x15
      10:      	ret
'''


def frames_of(listing: str) -> dict[str, int]:
    architecture: str
    functions: list[stack_frames.Function]
    architecture, functions = stack_frames.parse_listing(listing)
    frames: dict[str, int] = {}
    function: stack_frames.Function
    for function in functions:
        if function.name != function.section:
            frames[function.name] = stack_frames.frame_size(architecture, function)
    return frames


class PrologueTests(unittest.TestCase):
    def test_windows_probed_frame(self) -> None:
        # Eight pushes, then 0xc21d8 bytes through ___chkstk_ms; the
        # section's own label is not a function.
        self.assertEqual(frames_of(WINDOWS_X64), {'main': 8 * 8 + 0xc21d8})

    def test_linux_x64_frame_stops_at_the_first_call(self) -> None:
        frames: dict[str, int] = frames_of(LINUX_X64)
        # The push of rbp, four more pushes after rbp is copied, then the
        # sub; the sub after the call to consume is not prologue.
        self.assertEqual(frames['big_frame'], 6 * 8 + 0x10ca20)
        self.assertEqual(frames['tiny_frame(int)'], 8 + 0x40)

    def test_gnu_objdump_syntax_reads_the_same(self) -> None:
        self.assertEqual(frames_of(LINUX_X64_GNU), {'big_frame': 6 * 8 + 0x10ca20})

    def test_x64_stack_clash_loop(self) -> None:
        # The loop probes down to r11, then the remainder is subtracted.
        self.assertEqual(frames_of(LINUX_X64_STACK_CLASH), {'big_frame': 0x10c000 + 0x8e8})

    def test_arm64_realigned_frame_through_a_scratch_register(self) -> None:
        self.assertEqual(frames_of(LINUX_ARM64_REALIGNED), {'big_frame': 0x50 + 0x10c000 + 0xa10})

    def test_arm64_two_part_subtraction_stops_at_the_call(self) -> None:
        self.assertEqual(frames_of(LINUX_ARM64_PLAIN), {'big_frame': 0x20 + 0x10c000 + 0x8e0})

    def test_arm64_stack_clash_loop(self) -> None:
        self.assertEqual(frames_of(LINUX_ARM64_STACK_CLASH), {'big_frame': 0x20 + 0x10c000 + 0x8e0})

    def test_arm64_register_amount_built_by_movk(self) -> None:
        self.assertEqual(frames_of(MOVK_CONSTANT), {'probed': 0x10 + 0x48000})

    def test_macos_arm64_frames_and_symbol_names(self) -> None:
        frames: dict[str, int] = frames_of(MACOS_ARM64)
        self.assertEqual(frames['ltmp0'], 0x40 + 0x10c000 + 0xa20)
        self.assertEqual(frames['_middle_frame'], 0x40 + 0x49000 + 0x520)
        names: dict[tuple[str, int], str] = stack_frames.parse_symbols(MACOS_ARM64_SYMBOLS)
        self.assertEqual(names[('__TEXT,__text', 0)], '_big_frame')
        self.assertEqual(names[('__TEXT,__text', 0x12c)], '_middle_frame')

    def test_other_architectures_are_not_read(self) -> None:
        architecture: str
        architecture, _ = stack_frames.parse_listing('probe.o:\tfile format elf32-littlearm\n')
        self.assertEqual(architecture, '')

    def test_operands_split_outside_brackets(self) -> None:
        self.assertEqual(stack_frames.split_operands('x29, x30, [sp, #-0x50]!'),
                         ['x29', 'x30', '[sp, #-0x50]!'])
        self.assertEqual(stack_frames.split_operands('0x80(%rsp,%rax,8),%rbp'), ['0x80(%rsp,%rax,8)', '%rbp'])


class ObjectChoiceTests(unittest.TestCase):
    def test_only_target_objects_outside_third_party(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            tree: Path = Path(scratch)
            kept: list[str] = [
                'CMakeFiles/gui_forms_theme_tests.dir/tests/theme_tests.cpp.obj',
                'stx_png/CMakeFiles/stx_png.dir/src/decode.c.o',
            ]
            left_out: list[str] = [
                'third_party/harfbuzz/CMakeFiles/harfbuzz.dir/src/hb.cc.obj',
                'CMakeFiles/gui_forms_core.dir/third_party/vendored.cpp.o',
                'CMakeFiles/3.31.0/CompilerIdCXX/CMakeCXXCompilerId.o',
                'CMakeFiles/CMakeScratch/TryCompile-1/CMakeFiles/cmTC_1.dir/test.cpp.o',
                'CMakeFiles/gui_forms_core.dir/src/core/notes.txt',
            ]
            relative: str
            for relative in kept + left_out:
                path: Path = tree / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(b'')
            found: list[str] = []
            path_found: Path
            for path_found in stack_frames.first_party_objects(tree):
                found.append(path_found.relative_to(tree).as_posix())
            self.assertEqual(found, sorted(kept))



TARGETS_LISTING: str = """CMakeFiles/gui_forms_core.dir/src/core/a.cpp.obj: CXX_COMPILER__gui_forms_core_unscanned_RelWithDebInfo
CMakeFiles/gui_forms_core.dir/src/core/b.cpp.obj: CXX_COMPILER__gui_forms_core_unscanned_RelWithDebInfo
CMakeFiles/gui_forms_core.dir/src/core/c.c.obj: C_COMPILER__gui_forms_core_unscanned_RelWithDebInfo
gui_forms_theme_tests.exe: CXX_EXECUTABLE_LINKER__gui_forms_theme_tests_RelWithDebInfo
all: phony
"""

DRY_RUN: str = """[1/3] ccache clang++ -DGUI -O1 -fsanitize=address -o CMakeFiles/gui_forms_core.dir/src/core/b.cpp.obj -c D:/src/core/b.cpp
[2/3] ccache clang -O1 -o {tree}/CMakeFiles/gui_forms_core.dir/src/core/c.c.obj -c D:/src/core/c.c
[3/3] clang++ -fsanitize=address CMakeFiles/gui_forms_core.dir/src/core/a.cpp.obj -o gui_forms_theme_tests.exe
"""

REGENERATING: str = """[0/1] cmake.exe --regenerate-during-build -SD:/src -BD:/build
"""


class StaleObjectTests(unittest.TestCase):
    def objects(self, tree: Path, names: list[str]) -> list[Path]:
        paths: list[Path] = []
        name: str
        for name in names:
            paths.append(tree / ('CMakeFiles/gui_forms_core.dir/src/core/' + name))
        return paths

    def test_listing_names_object_outputs_only(self) -> None:
        self.assertEqual(stack_frames.listed_objects(TARGETS_LISTING), {
            'CMakeFiles/gui_forms_core.dir/src/core/a.cpp.obj', 'CMakeFiles/gui_forms_core.dir/src/core/b.cpp.obj',
            'CMakeFiles/gui_forms_core.dir/src/core/c.c.obj'})

    def test_dry_run_names_the_objects_it_would_compile(self) -> None:
        # The second command names its object by an absolute path in the tree.
        tree: Path = Path(tempfile.gettempdir()) / 'build'
        pending: set[str] | None = stack_frames.pending_objects(DRY_RUN.replace('{tree}', tree.as_posix()), tree)
        self.assertEqual(pending, {'CMakeFiles/gui_forms_core.dir/src/core/b.cpp.obj',
                                   'CMakeFiles/gui_forms_core.dir/src/core/c.c.obj'})
        self.assertIsNone(stack_frames.pending_objects(REGENERATING, Path('D:/build')))

    def test_orphaned_and_out_of_date_objects_are_set_apart(self) -> None:
        tree: Path = Path(tempfile.gettempdir()) / 'build'
        objects: list[Path] = self.objects(tree, ['a.cpp.obj', 'b.cpp.obj', 'c.c.obj', 'removed.cpp.obj'])
        choice: stack_frames.ObjectChoice = stack_frames.choose_objects(
            tree, objects, TARGETS_LISTING, DRY_RUN.replace('{tree}', tree.as_posix()))
        self.assertEqual(choice.current, self.objects(tree, ['a.cpp.obj']))
        self.assertEqual(choice.out_of_date, self.objects(tree, ['b.cpp.obj', 'c.c.obj']))
        self.assertEqual(choice.orphaned, self.objects(tree, ['removed.cpp.obj']))
        self.assertEqual(choice.note, '')

    def test_a_tree_needing_regeneration_still_sets_orphans_apart(self) -> None:
        tree: Path = Path('D:/build')
        objects: list[Path] = self.objects(tree, ['a.cpp.obj', 'b.cpp.obj', 'removed.cpp.obj'])
        choice: stack_frames.ObjectChoice = stack_frames.choose_objects(tree, objects, TARGETS_LISTING,
                                                                        REGENERATING)
        self.assertEqual(choice.current, self.objects(tree, ['a.cpp.obj', 'b.cpp.obj']))
        self.assertEqual(choice.orphaned, self.objects(tree, ['removed.cpp.obj']))
        self.assertIn('regenerating', choice.note)

    def test_without_ninja_every_object_is_read(self) -> None:
        tree: Path = Path('D:/build')
        objects: list[Path] = self.objects(tree, ['a.cpp.obj', 'removed.cpp.obj'])
        choice: stack_frames.ObjectChoice = stack_frames.choose_objects(tree, objects, None, None)
        self.assertEqual(choice.current, objects)
        self.assertEqual(choice.orphaned, [])
        self.assertIn('every object read', choice.note)


class BuildRecordTests(unittest.TestCase):
    """A run that built nothing has nothing to check; a run that built something must leave objects."""

    def check(self, build: Path) -> int:
        # The disassembler is never started: these trees hold no object.
        return stack_frames.main(['check', '--tree', str(build / 'gui-forms-sanitize'), '--objdump', 'unused'])

    def test_nothing_built_passes_without_a_tree(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            build: Path = Path(scratch)
            ci_build.write_build_record(build / 'gui-forms-sanitize', ci_build.BUILT_NOTHING,
                                        'no test reads the 1 paths changed since origin/main')
            self.assertEqual(self.check(build), 0)

    def test_a_selection_that_built_but_left_no_object_fails(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            build: Path = Path(scratch)
            tree: Path = build / 'gui-forms-sanitize'
            (tree / 'CMakeFiles').mkdir(parents=True)
            ci_build.write_build_record(tree, ci_build.BUILT_SELECTED, '3 build outputs of the selected tests')
            self.assertEqual(self.check(build), 2)

    def test_an_empty_tree_without_a_record_fails(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            build: Path = Path(scratch)
            (build / 'gui-forms-sanitize').mkdir()
            self.assertEqual(self.check(build), 2)

if __name__ == '__main__':
    unittest.main()
