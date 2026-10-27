#!/usr/bin/env python3
"""Moved to template_harness: harness/compiler_identity.py (harness.lock pins the commit).

A forwarding shim, kept so the commands and imports that name this path
(File Manager's among them) keep working: run, it runs the harness's tool
with these arguments; imported, it is the harness's module. Either way it
fails clearly when the harness is not beside this checkout
(tools/harness_link.py, docs/building/template-harness.md).
"""
import harness_link

if __name__ == '__main__':
    harness_link.run_tool('compiler_identity')
else:
    harness_link.forward_import('compiler_identity')
