#!/usr/bin/env python3
"""Where GUI.Forms finds template_harness, which holds its shared CI tools.

The CI tools GUI.Forms shares with other products (the reuse, promotion,
cache, toolchain and checker tools) live in malkuth-software/template_harness
(owner decision 2). harness.lock at the root of this checkout pins the
commit, and the harness is checked out beside this checkout:

    <workspace>/gui_forms           this repository
    <workspace>/template_harness     the pinned harness (harness.lock)

CI checks it out with .github/actions/harness-checkout; locally, from the
workspace, `python3 template_harness/harness/lock.py --lock
gui_forms/harness.lock checkout template_harness` (the first time: clone
the repository there, then run it), or the harness's bootstrap.
docs/building/template-harness.md explains the layout.

Importing this module:

- fails with a message naming the pinned commit when the harness is not
  beside this checkout;
- puts the harness's harness/ directory first on sys.path, so its modules
  import each other (and this checkout's tools import them) by name;
- sets HARNESS_PROJECT to the harness's projects/gui_forms.toml and
  HARNESS_WORKSPACE to the workspace, unless the caller set them, so the
  harness tools read GUI.Forms' names exactly as the moved tools did.

The forwarding shims at the old tool paths (tools/ci_cache.py and the rest,
listed in SHIMS) call forward_import when imported and run_tool when run,
so commands and imports that name the old paths keep working.
"""
from __future__ import annotations
import importlib.util
from importlib.machinery import ModuleSpec
import os
from pathlib import Path
import re
import runpy
import sys
from types import ModuleType

SOURCE: Path = Path(__file__).resolve().parents[1]
WORKSPACE: Path = SOURCE.parent
HARNESS: Path = WORKSPACE / 'template_harness'
TOOLS: Path = HARNESS / 'harness'
PROJECT_FILE: Path = HARNESS / 'projects' / 'gui_forms.toml'
LOCK_FILE: Path = SOURCE / 'harness.lock'
LOCK_COMMIT: re.Pattern[str] = re.compile(r'^commit\s*=\s*"([0-9a-f]{40})"', re.MULTILINE)
# The old tool paths that forward to the harness: tools/<name>.py, plus the
# shell script tools/install_linux_ci_packages.sh, which forwards by itself.
SHIMS: tuple[str, ...] = (
    'audit_macos_minimum', 'build_macos_runtimes', 'cache_manifest', 'change_selection', 'check_house_style',
    'check_lifetime_rules', 'check_state_machines', 'ci_cache', 'ci_promote', 'ci_reuse', 'ci_sanitizer_seed',
    'ci_tree_receipt', 'compiler_identity', 'macos_runtime_manifest', 'native_tests', 'prune_cache_releases',
    'publish_cache', 'windows_toolchain')


class HarnessMissing(SystemExit):
    """The harness is not beside this checkout; the message says how to put it there."""


def pinned_commit() -> str:
    """The commit harness.lock pins, or a note that it cannot be read."""
    if not LOCK_FILE.is_file():
        return '(no ' + str(LOCK_FILE) + ')'
    found: re.Match[str] | None = LOCK_COMMIT.search(LOCK_FILE.read_text(encoding='utf-8'))
    if found is None:
        return '(' + str(LOCK_FILE) + ' names no commit)'
    return found.group(1)


def missing_message() -> str:
    message: str = ('GUI.Forms: template_harness is not beside this checkout. Expected ' + str(TOOLS)
                    + ' at commit ' + pinned_commit() + ' (gui_forms/harness.lock). From ' + str(WORKSPACE)
                    + ': git clone https://github.com/malkuth-software/template_harness template_harness, then '
                    'python3 template_harness/harness/lock.py --lock gui_forms/harness.lock checkout template_harness'
                    ' (docs/building/template-harness.md).')
    return message


def require() -> None:
    """Fail clearly without the harness; otherwise put its tools on sys.path and name the project."""
    if not (TOOLS / 'project.py').is_file() or not PROJECT_FILE.is_file():
        raise HarnessMissing(missing_message())
    tools: str = str(TOOLS)
    if tools in sys.path:
        sys.path.remove(tools)
    sys.path.insert(0, tools)
    os.environ.setdefault('HARNESS_PROJECT', str(PROJECT_FILE))
    os.environ.setdefault('HARNESS_WORKSPACE', str(WORKSPACE))


def forward_import(name: str) -> None:
    """Make the module being imported as name the harness's module of that name.

    Called from a shim while the import system executes it: the harness
    module replaces the shim in sys.modules, and the import statement that
    loaded the shim binds the module it then finds there.
    """
    path: Path = TOOLS / (name + '.py')
    spec: ModuleSpec | None = importlib.util.spec_from_file_location(name, path)
    if spec is None or spec.loader is None:
        raise ImportError('template_harness has no module ' + name + ' at ' + str(path))
    module: ModuleType = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)


def run_tool(name: str) -> None:
    """Run the harness's tool of that name as the main program, with this process's arguments."""
    runpy.run_path(str(TOOLS / (name + '.py')), run_name='__main__')


require()
