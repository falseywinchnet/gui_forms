"""Check that a stable ID literal is validated by an actual C++ compiler."""
from pathlib import Path
import subprocess
import sys


def compile_source(compiler: str, include: Path, body: str) -> subprocess.CompletedProcess[str]:
    source: str = ('#include <gui_forms/control/stable_id/stable_id.hpp>\n'
                   '#include <string>\n'
                   'int main() {\n' + body + '\n}\n')
    return subprocess.run(
        [compiler, '-std=c++20', '-fsyntax-only', '-x', 'c++', '-I', str(include), '-'],
        input=source, text=True, capture_output=True, check=False)


def main() -> int:
    compiler: str = sys.argv[1]
    include: Path = Path(sys.argv[2])
    accepted: str = ('gui_forms::StableId a("paint.editor"); gui_forms::StableId b("list-view.item.0");\n'
                     'gui_forms::StableId c(std::string("x")); static_cast<void>(a == b && b == c);')
    result: subprocess.CompletedProcess[str] = compile_source(compiler, include, accepted)
    if result.returncode != 0:
        print(result.stderr, file=sys.stderr)
        raise RuntimeError('valid stable ID literals must compile')
    literal_diagnostic: str = 'stable_id_literal_is_not_dot_separated_lowercase_dash_segments'
    cases: list[tuple[str, str]] = [
        ('gui_forms::StableId id("list_view");', literal_diagnostic),
        ('gui_forms::StableId id("");', literal_diagnostic),
        ('gui_forms::StableId id("a..b");', literal_diagnostic),
        ('gui_forms::StableId id("Upper");', literal_diagnostic),
        ('const char* text = "x"; gui_forms::StableId id(text);', 'no matching constructor'),
    ]
    for body, expected in cases:
        result = compile_source(compiler, include, body)
        if result.returncode == 0 or expected not in result.stderr:
            print(result.stderr, file=sys.stderr)
            raise RuntimeError('missing deliberate-failure diagnostic for: ' + body)
    print(str(len(cases)) + ' deliberate-failure diagnostics passed')
    return 0


if __name__ == '__main__':
    sys.exit(main())
