"""Check the public diagnostics against an actual C++ compiler."""
from pathlib import Path
import subprocess
import sys


def main() -> int:
    compiler: str = sys.argv[1]
    include: Path = Path(sys.argv[2])
    prefix: str = '''#include <gui_forms/event.hpp>
#include <array>
#include <string>
struct Owner : gui_forms::Component {
    void click() {}
    void index(int) {}
    void mutate(int&) {}
    void text(std::string) {}
    void large(std::array<int, 5>) {}
};
int main() { gui_forms::Event<int> event; Owner owner;
'''
    cases: list[tuple[str, str]] = [
        ('gui_forms::on(event, owner, &Owner::mutate);', 'handler must accept bound values'),
        ('gui_forms::on(event, owner, &Owner::text);', 'handler must accept bound values'),
        ('gui_forms::on(event, owner, &Owner::text, std::string("x"));', 'must be trivially copyable'),
        ('gui_forms::on(event, owner, &Owner::large, std::array<int, 5>{});', 'must total at most 16 bytes'),
        ('gui_forms::on(event, owner, &Owner::click, 1, 2, 3);', 'at most two bound values'),
    ]
    body: str = ""
    expected: str = ""
    for body, expected in cases:
        source: str = prefix + body + '\n}\n'
        result: subprocess.CompletedProcess[str] = subprocess.run(
            [compiler, '-std=c++20', '-fsyntax-only', '-x', 'c++', '-I', str(include), '-'],
            input=source, text=True, capture_output=True, check=False)
        if result.returncode == 0 or expected not in result.stderr:
            print(result.stderr, file=sys.stderr)
            raise RuntimeError('missing deliberate-failure diagnostic: ' + expected)
    print('5 deliberate-failure diagnostics passed')
    return 0


if __name__ == '__main__':
    sys.exit(main())
