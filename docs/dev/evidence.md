# Evidence index

GUI.Forms keeps its measurements, experiment reports and plans in the
repository rather than in a wiki, and it keeps negative results next to
positive ones. These directories are internal working material: their tone
is terse, they use the project's internal labels for claims, and they are
not rebuilt into this site. They are listed here so that a reader who wants
the data behind a statement in the user documentation knows where to look.

`results/`
: Dated measurement folders, one per experiment or checkpoint. Each contains
  the raw numbers (JSON, CSV or logs), the exact commands, the environment
  and a short README. Examples: label layout and wrap reuse, raster-cache
  checkpoints, prepared-window shaping and placement, Windows text latency,
  and the host-service and dialog traces for the AppKit adapter. When a
  concept page says a counter proves something, the proof is here.

`experiments/`
: Narrative reports (about 150 files) recording what was tried, what was
  observed, what was measured and what was rejected, in the order it
  happened. They cover text rasterization profiles, Unicode text store and
  grapheme conformance, the compiler-cache proof, audio decoding and
  transport, the compatibility facade and its Wine runs, the drawing core,
  initialization ownership, document-view benchmarks and many smaller
  probes. A report is evidence only for the behaviours it exercised.

`planning/`
: The implementation programme and its ledgers: the master plan with its
  dependency-ordered milestones, the control completeness matrix (the
  authoritative supported / deferred / excluded ledger), the Forms API
  catalogue and behaviour-gap catalogue, the paint-pipeline availability
  matrix, the consumer capability profiles, the programming house style, and
  dated stage plans such as the current polish-stage plan. When this site's
  [Roadmap](roadmap) summarises a gate, the full statement is in one of
  these files.

`manifests/`
: Machine-readable consumption snapshots for downstream projects, with
  artefact digests and the commands that produced them.

`docs/notes/`
: Dated working notes that were once mixed with the user documentation:
  interface negotiations, proposals, audits and development records. They are
  kept for provenance and are excluded from this site.

`decisions/`
: The architecture decision records, rendered under
  [Architecture decisions](decisions).

## How to read a claim

The internal documents label statements as given (a constraint handed down),
observed (seen but not measured), measured (numbers with a recorded method),
hypothesis, candidate, rejected or decided. This site uses plain prose, but
the distinction still applies: a sentence here that describes behaviour is
backed by a test under `tests/`; a sentence that describes cost is backed by
a folder under `results/`; and a sentence that describes intent points at
`planning/`.
