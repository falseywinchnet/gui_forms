# Stage FD: form documents

Release: 0.4 Flexible ("form documents that round-trip"). Depends on: T1
(the schema they serialize) and T3 (shared syntax and round-trip machinery).
Unblocks D, and through it V. Status 2026-10-13: not started; format decided. Authority: `planning/FORWARD_PROGRAMME.md` (stage D
names "form documents" as a dependency, and §1 states the no-clobber rule).
The forward programme has no separate section for this stage; this brief and
the stage index added it on 2026-10-12 to match the public roadmap.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "the VS WinForms designer is garbage. Hand editing
should be easy and legal and the visual tool updates or vice versa. No more
clobber, no more breakage." And: "a program's GUI write-out can declare
z-order, layers, objects, properties, state machines with a compact
specification, and a theme can override this with properties by itself or
addressing the engine." The owner also called the Windows Forms split of
layout, hook-up and handler files "both correct and at the same time perhaps
a bit sloppy". The C++ dot API that sets properties and registers
interactions and the form document are two views of one schema.

The interview already settled the principle (GF014): the form description is
authoritative and round-trippable; generated code is disposable; production
keeps compiled schema, stable IDs and inspection metadata, not necessarily
source text. Delphi `.dfm`, Interface Builder `.xib`, Qt Designer `.ui` and
GtkBuilder files work this way. Visual Studio's Windows Forms designer is the
odd one out: `InitializeComponent` is its saved form, which is why hand edits
get clobbered. For C# users (stage S), `InitializeComponent` is generated from
the document, so the familiar file still appears.

## What exists today

- `demo/gallery.dml`: a provisional compiled form description in a compact,
  CSS-like block syntax (`form gallery.root { title: "…"; button
  gallery.command.reset { text: "Reset"; } }`), with stable IDs on every
  node. `AGENTS.md` keeps it as "provisional evidence only". The static tree
  builder is `include/gui_forms/control/static_tree/`.
- Interview GF014's 2026-08-10 refinement gave the authoritative visual
  source to Web.Forms as a bounded HTML/CSS profile. Web.Forms is no longer
  maintained; the owner: "there is no sibling Web.Forms project, or at least
  there is nobody managing it anymore — it is free to capture."
- `planning/CONCISE_AUTHORING_AVAILABILITY.md` deliveries C and E: options
  records per control, transactional attach, a versioned authoring manifest,
  and a candidate round-trip policy. Under that policy only a declared data
  region is machine-editable, unknown bytes are preserved verbatim or the
  edit is refused with an exact diagnostic, and handlers stay outside
  generated regions.
- The property registry and control manifest (see T1).

## What to deliver

1. A form-document format that serializes the T1 schema: objects with stable
   IDs, z-order, layers (control plane and backplane), properties, layout
   containers from L, presentational state machines, and references to
   named handlers (never handler code).
2. A parser and serializer with the same guarantees as T3: diagnostics by
   file, line and node; bounded hostile-input handling; a byte-preserving
   round trip of untouched documents; edits that change only the bytes of
   the nodes they touch.
3. Disposable code generation to C++ (and later C# for S) that compiles
   against the installed SDK and passes the house-style check.
4. Validation before mutation: a document that names an unknown control,
   property or state is rejected with an exact diagnostic before anything is
   built.

## How

- One schema, many projections: the document must not invent anything the
  C++ API cannot express, and vice versa.
- Keep themes and forms separate. A form says what exists and where; a theme
  says how it looks. Inline style in a form uses the T3 syntax and sits at
  its place in the cascade (T2).
- House style: the parser fills caller-owned node tables (§4); stable IDs,
  node indices and byte offsets are distinct types (§1); failure leaves the
  previous document model intact (§9); generated code follows the stricter
  generated-code profile of house style §10 (named listeners, typed
  identities, deterministic initialization and connection order, no
  capture-backed callables).

## Gate

Accepted by the owner on 2026-10-13: a corpus of documents round-trips byte for byte
when untouched; a scripted edit of one property changes only that
property's bytes; every Gallery page can be expressed as a document whose
generated C++ compiles against the installed SDK and passes its tests. D's
gate then adds the builder.

## Owner decisions, 2026-10-13

1. **Syntax: the CSS family, shared with themes.** One parser and one
   round-trip machinery serve form and theme documents. The provisional
   Gallery DML's block style already looks like this and evolves into it.
2. **The Gallery DML migrates** in this stage; the Gallery becomes the first
   real form-document corpus and test.
3. **The gate above is accepted.**

## Read first

- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF014–GF016
- `planning/CONCISE_AUTHORING_AVAILABILITY.md` deliveries C and E
- `demo/gallery.dml`, `include/gui_forms/control/static_tree/`
- [T1](../T1-anatomy/README.md), [T3](../T3-theme-language/README.md),
  [D](../D-interface-builder/README.md) briefs
