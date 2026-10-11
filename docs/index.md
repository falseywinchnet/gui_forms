# GUI.Forms documentation

GUI.Forms is a retained, CPU-rendered forms toolkit written in C++20. You
build a tree of controls once, mutate it imperatively, and the toolkit tracks
exactly which layout, paint and hit-test work that mutation requires. Painting
is damage-limited and runs on the CPU; there is no GPU path and no redraw
loop. Text is shaped by HarfBuzz and rasterized by FreeType from bundled fonts,
so the same string renders identically on every platform.

Current status, stated plainly: the core is retained and portable; native
hosts exist for macOS (AppKit), Windows (Win32) and Linux (X11, enabled with
a configure option); and the public API is not yet stable. The release
pipeline packages three demonstration programs for all three platforms; see
[Download](start/download).

```{toctree}
:maxdepth: 1
:caption: Start here

start/install
start/consume-the-sdk
start/first-window
start/build-from-source
start/download
```

```{toctree}
:maxdepth: 1
:caption: Controls

controls/index
```

```{toctree}
:maxdepth: 1
:caption: Concepts

concepts/retained-tree
concepts/update-scopes-and-dirtiness
concepts/layout
concepts/painting-and-damage
concepts/text-engine
concepts/input-and-focus
concepts/threading
concepts/lifecycle
concepts/hosts-and-capabilities
```

```{toctree}
:maxdepth: 1
:caption: Reference

api/index
```

```{toctree}
:maxdepth: 1
:caption: Project

project/decisions
project/roadmap
project/contributing
project/evidence
```
