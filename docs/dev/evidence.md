# Evidence index

GUI.Forms keeps its measurements, experiment reports and plans in a separate
repository, [`malkuth-software/notes`](https://github.com/malkuth-software/notes/tree/gui_forms), on its
`gui_forms` branch, and it keeps negative results next to positive ones. That
material is internal: its tone is terse, it uses the project's internal
labels for claims, and it is not rebuilt into this site. It is listed here so
that a reader who wants the data behind a statement in the user documentation
knows where to look. A source comment that cites `notes:<path>` names a path
on that branch.

[`results/`](https://github.com/malkuth-software/notes/tree/gui_forms/results)
: Dated measurement folders, one per experiment or checkpoint. Each contains
  the raw numbers (JSON, CSV or logs), the exact commands, the environment
  and a short README. When a concept page says a counter proves something,
  the proof is here.

[`experiments/`](https://github.com/malkuth-software/notes/tree/gui_forms/experiments)
: Narrative reports recording what was tried, what was observed, what was
  measured and what was rejected, in the order it happened. A report is
  evidence only for the behaviours it exercised.

[`planning/`](https://github.com/malkuth-software/notes/tree/gui_forms/planning)
: The implementation programme and its ledgers: the forward programme and
  its stage briefs, the control completeness matrix, the Forms API catalogue
  and behaviour-gap catalogue, the consumer capability profiles and the
  design language. When this site's [Roadmap](roadmap) summarises a gate, the
  full statement is in one of these files.

[`decisions/`](https://github.com/malkuth-software/notes/tree/gui_forms/decisions)
: The architecture decision records, listed under
  [Architecture decisions](decisions).

[`docs-notes/`](https://github.com/malkuth-software/notes/tree/gui_forms/docs-notes)
: Dated working notes: interface negotiations, proposals, audits and
  development records.

`manifests/`
: In this repository: machine-readable consumption snapshots for downstream
  projects, with artefact digests and the commands that produced them.

## How to read a claim

The internal documents label statements as given (a constraint handed down),
observed (seen but not measured), measured (numbers with a recorded method),
hypothesis, candidate, rejected or decided. This site uses plain prose, but
the distinction still applies: a sentence here that describes behaviour is
backed by a test under `tests/`; a sentence that describes cost is backed by
a folder under `results/` in the notes repository; and a sentence that
describes intent points at its `planning/`.
