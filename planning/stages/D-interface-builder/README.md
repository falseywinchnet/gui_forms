# Stage D: interface builder

Release: 0.7 Designers. Depends on: T3, L, FD. Unblocks V (which needs D's
core). Status 2026-10-12: not started. Authority:
`planning/FORWARD_PROGRAMME.md` §6 D.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "Visual Studio and GTK and others all had visual
tools for laying out controls. We should aim to build our own and integrate
it into Visual Studio as a plugin. This will also happen remotely" (on Shadow
Windows). It is "dogfed": written in GUI.Forms. And: "the VS WinForms
designer is garbage. Hand editing should be easy and legal and the visual
tool updates or vice versa. No more clobber, no more breakage."

## The model the owner accepted

- The builder saves and reads form documents (FD), the serialization of the
  one schema (T1). C++ and C# `InitializeComponent` are generated from them
  and are disposable (interview GF014).
- Hosting GUI.Forms controls inside Visual Studio's own Windows Forms
  designer is a dead end: for modern .NET it runs out of process, renders real
  HWND controls, and needs Microsoft's designer SDK for custom designers. The
  proven model is a standalone designer that edits a form file, a code
  generator, and a thin Visual Studio extension that opens forms and runs the
  generator. Qt Designer with Qt VS Tools works this way, and so do Glade and
  Cambalache for GTK.
- The owner's per-window custom-control flow starts here: give a control a
  stable ID in the builder, save the project, open the theme in the theme
  designer (V), find the control by that ID, start from its type's template,
  adjust and save a rule keyed on the ID.

Prior art to study, for what to take from each:

| Tool | Take |
|---|---|
| Delphi and Lazarus (`.dfm`, Object Inspector) | the property inspector and event wiring |
| Xcode Interface Builder | constraints; outlets and actions as the code connection |
| Qt Designer, Qt VS Tools | standalone designer, form file, generator, thin IDE extension |
| GtkBuilder tools (Glade, Cambalache) | the same split for GTK |
| component-and-variant design tools | components, variants and automatic layout for dynamic flex |

## What to deliver

1. A standalone builder written in GUI.Forms that edits form documents:
   objects, z-order, layers, properties, layout containers (L),
   presentational state machines and stable IDs.
2. Generation of disposable C++ and C# code.
3. A thin Visual Studio extension that opens form documents in the builder
   and runs the generator.
4. Round-trip safety: a document edited by hand, then in the builder, keeps
   every untouched byte; an edit the model cannot represent is refused with
   an exact diagnostic, never written over.

## How

- Dogfood: the builder is a GUI.Forms program on the default theme (T4). Do
  not hand-style it; problems with its look are toolkit problems.
- The builder may compose persistent panels; the owner retired ADR-013 on
  2026-10-13.
- The builder is also one of the native consumers for the 1.0 ABI
  conformance condition (stage C): build it against the installed SDK.
- Generated code passes the house-style check and follows the generated-code
  profile of house style §10.
- Hand-written handlers stay outside generated regions; a regex rewrite of
  arbitrary C++ is never a preservation guarantee
  (`planning/CONCISE_AUTHORING_AVAILABILITY.md` delivery E).

## Gate

Hand edits and builder edits round-trip: a document edited by hand, then in
the builder, keeps every untouched byte; the builder can author a Gallery
page that compiles and passes its tests. Record under `results/`.

## Owner decisions, 2026-10-13

1. **ADR-013 is retired.** The builder may use persistent panels (toolbox,
   canvas, inspector).

Still open, minor: where the Visual Studio extension lives and how it is
distributed.

## Read first

- [FD](../FD-form-documents/README.md), [L](../L-layout/README.md),
  [T3](../T3-theme-language/README.md) briefs
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF014–GF016
- `planning/CONCISE_AUTHORING_AVAILABILITY.md` deliveries C and E
- `decisions/ADR-013-panel-composition-reserved-to-file-manager.md` (retired)
