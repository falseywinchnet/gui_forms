# A Diagram control for GUI.Forms: study

Date: 2026-10-18. Status: research study for matrix row 55
(`planning/CONTROL_COMPLETENESS_MATRIX.md`; W brief, "Advanced candidates,
2026-10-14"), written by worker 2 for the coordinator. It is not a decision
record and proposes no code. Nothing here is decided until the owner answers
section 8.

Matrix row 55, as it stands (Known): "`Diagram`: nodes, ports, connectors,
selection, zoom and pan over retained painting and hit-testing", admitted by
the owner on 2026-10-14 as a candidate that "needs its own study before any
plan"; "No graph-layout engine or file format promised before the study";
dependencies "`DrawingSurface`, hit-testing, input, layout transactions".

**Reading key**, as in the docking and ribbon studies. **Known** means read in
a cited source during this study (a vendor page, a project's own
documentation or source, a file in the repository). **Inferred** means
reasoned from known material but stated by no source. **Unverified** means
remembered or reported second hand (including search-engine summaries that
were not opened at the primary page), not checked against a primary source
here. Repository paths are relative to the repository root; code that exists
only on a branch names the branch. Sources were read on the web on
2026-10-18; nothing was downloaded or built.

Contents

1. What a diagram control is
2. The canonical systems, one by one
3. Side by side
4. What GUI.Forms has today
5. A proposed model in GUI.Forms terms
6. The hard parts
7. Phased plan
8. Open owner decisions, as interview questions

Sources, by short name:

| Name | Source |
|---|---|
| GOJS-INTRO | GoJS, "Introduction", https://gojs.net/latest/intro/index.html (Diagram, Model, GraphLinksModel, Part, Panel, layouts; Known) |
| GOJS-PORTS | GoJS, "Ports in Nodes", https://gojs.net/latest/intro/ports.html (Known) |
| GOJS-PERF | GoJS, "Improving Performance", https://gojs.net/latest/intro/performance.html (Known) |
| GOJS-A11Y | GoJS, "Accessibility", https://gojs.net/latest/learn/accessibility (Known) |
| GOJS-TX | GoJS, "Transactions and the UndoManager", https://gojs.net/latest/learn/transactions (title and existence Known from search; contents Unverified) |
| GODIAGRAM | Northwoods GoDiagram for Windows Forms reference, `GoUndoManager`, `GoDocument.UndoManager`, https://nwoods.com/godiagram/docs/GoWin5.0.0/ and GoWin5.2.0 (search summaries of the reference pages; Unverified in detail) |
| YF-INTRO | yFiles for HTML Developer's Guide, Introduction, https://docs.yworks.com/yfiles-html/dguide/introduction/ (Known) |
| YF-GRAPH | yFiles, "The Graph Model", https://docs.yworks.com/yfiles-html/dguide/graph/ (Known) |
| YF-EDIT | yFiles `EdgeSnapLineProvider`, `OrthogonalEdgeEditingContext` usages, https://docs.yworks.com/yfiles-html/api/ (search summaries; Unverified in detail) |
| DX-CONN | DevExpress WinForms Diagram, "Connectors", https://docs.devexpress.com/WindowsForms/116884/controls-and-libraries/diagrams/diagram-items/connectors (Known) |
| DX-SAVE | DevExpress WinForms Diagram, "Saving and Loading Diagrams", https://docs.devexpress.com/WindowsForms/118179/controls-and-libraries/diagrams/saving-and-loading-diagrams (Known) |
| DX-LAYOUT | DevExpress `DiagramControl.ApplyTreeLayout`, `ApplyMindMapTreeLayout`, `ApplyTipOverTreeLayout`, `ApplySugiyamaLayout` API pages, https://docs.devexpress.com/WindowsForms/ (names Known from search; behaviour Unverified) |
| DX-BLOG | DevExpress blog, "WinForms and WPF Diagram Control (Coming soon in v15.2)", 2015 (search summary; Unverified) |
| TEL-ROUTE | Telerik RadDiagram for WinForms, "Routing", https://www.telerik.com/products/winforms/documentation/controls/diagram/features/routing (Known) |
| SF | Syncfusion WinForms Diagram product pages, https://www.syncfusion.com/winforms-ui-controls/diagram and the Diagram SDK overview https://help.syncfusion.com/diagram-sdk/overview (search summaries; Unverified) |
| MF | MindFusion.Diagramming for WinForms: feature listing (componentsource), `Diagram.RouteAllLinks` and `RouteLinks` reference, https://mindfusion.eu/onlinehelp/winformspack/ (search summaries; Unverified in detail) |
| MSAGL | Microsoft Automatic Graph Layout, https://github.com/microsoft/automatic-graph-layout README (Known); algorithm and routing names from the Syncfusion MSAGL blog and the msagljs README (search summaries; Unverified) |
| VISIO-GLUE | Microsoft Visio support, "Edit connector lines, arrows, or points", and VBA reference `Cell.GlueTo`, https://learn.microsoft.com/en-us/office/vba/api/Visio.Cell.GlueTo (search summaries; Unverified in detail) |
| VISIO-STRUCT | Visio 2010 Structured Diagram API: `ContainerProperties`, `LockMembership`, `InsertListMember`, https://learn.microsoft.com/en-us/previous-versions/office/developer/office-2010/ (search summaries; Unverified in detail) |
| VISIO-LAYER | Microsoft Visio support, "Use layers to set properties for multiple shapes" (search summary; Unverified) |
| MX-MANUAL | mxGraph User Manual (v4.2.2), https://jgraph.github.io/mxgraph/docs/manual.html (Known) |
| MX-API | mxGraph JavaScript API index (mxEdgeStyle, mxConnectionConstraint, mxCodec, mxUndoableEdit), unpkg mirror of 3.7.5 (search summaries; Unverified) |
| JJ-FEAT | JointJS 4.3, "Features", https://docs.jointjs.com/learn/features/ (Known) |
| JJ-BASICS | JointJS, "Diagram basics", https://docs.jointjs.com/learn/features/diagram-basics/ (Known) |
| JJ-ROUTE | JointJS API, routers and connectors, https://docs.jointjs.com/api/routers/, https://docs.jointjs.com/api/connectors/ (search summaries; Unverified in detail) |
| ARIA-GFX | W3C, WAI-ARIA Graphics Module 1.0, https://www.w3.org/TR/graphics-aria-1.0/ (Known) |
| KENDO-ARIA | Progress Kendo UI diagram ARIA notes, `diagram_aria.md` in `@progress/wct-a11y-spec` (search summary; Unverified) |
| BENTHIC | "Benthic: Perceptually Congruent Structures for Accessible Charts and Diagrams", MIT Visualization Group, 2025 (search summary; Unverified) |
| GSK | Balik et al., "GSK: ... graph sketching for blind users", 2014, https://ciigar.csc.ncsu.edu/files/bib/Balik2014-AccessGraphs.pdf (search summary; Unverified) |
| DOCK-STUDY | `planning/stages/DK-docking/DOCKING_STUDY.md` (form of this study; §5.3 layout transactions) |
| DOCK-PLAN | `planning/stages/DK-docking/PLAN.md` (accepted 2026-10-17) |
| RIBBON-STUDY | `planning/stages/RB-ribbon/RIBBON_STUDY.md` (form; §8 question style) |
| SPEC | `planning/DESIGN_LANGUAGE_SPECIFICATION.md` (§8 atoms, §12.7 the ruling) |
| HOUSE | `planning/PROGRAMMING_HOUSE_STYLE.md` |

Products not opened at a primary page in this study, and so reported only as
Unverified: Syncfusion, MindFusion, GoDiagram, Visio's object model, and the
detail of yFiles' editing and routing, MSAGL's algorithms, mxGraph's edge
styles and JointJS' routers. The owner may want a second pass with the
primary pages opened before decisions that depend on them (question A4).

---

## 1. What a diagram control is

A diagram control shows and edits a graph drawn on a two-dimensional plane:
items with positions and sizes (nodes), the lines that join them
(connectors), the places on a node where a connector may attach (ports), and
items that hold other items (containers). Every system read separates the
data from the view in some form (Known: GoJS "A Diagram can be thought of as
a view of a Model", and models "not Diagrams, are what you load and then
save after editing", GOJS-INTRO; JointJS "Graph is a model ... Paper is the
view where graph model is rendered", JJ-BASICS; mxGraph `mxGraphModel`
behind `mxGraph`, MX-MANUAL).

### 1.1 The canonical concepts

| Concept | What it is | Where it appears |
|---|---|---|
| Model | The graph's data, saved and loaded, edited through transactions | GoJS `Model`/`GraphLinksModel` (Known, GOJS-INTRO); yFiles `IGraph` (Known, YF-GRAPH); JointJS `Graph` (Known, JJ-BASICS); mxGraph `mxGraphModel` (Known, MX-MANUAL) |
| View | Renders the model in a viewport with zoom and pan, owns selection and tools | GoJS `Diagram`; yFiles `GraphComponent` (name Known from YF-INTRO navigation); JointJS `Paper`; mxGraph `mxGraph`/`mxGraphView`; MSAGL `GViewer` (WinForms) and the WPF control, both listing "Pan and Zoom" (Known, MSAGL) |
| Node (shape, vertex, element) | An item with bounds, content and ports | GoJS `Node`; yFiles `INode`; DevExpress shapes; mxGraph vertex `mxCell`; JointJS `Element` |
| Connector (link, edge) | A line between two endpoints, each attached to a port, a node or a free point, with bends and labels | GoJS `Link`; yFiles `IEdge` with `IBend`s; DevExpress `DiagramConnector` with `Points` (turn points), `BeginItem`/`EndItem` (Known, DX-CONN); mxGraph edge `mxCell`; JointJS `Link` |
| Port (connection point) | A named place on a node where connectors attach, with rules (which sides, how many links) | GoJS `portId`, `fromLinkable`/`toLinkable`, `fromSpot`/`toSpot`, `toMaxLinks` (Known, GOJS-PORTS); yFiles `IPort` ("Edges connect to nodes via ports"; "Ports can also link edges to other edges", Known, YF-GRAPH); DevExpress connection points with point and dynamic glue (Known, DX-CONN); Visio connection points and glue (Unverified, VISIO-GLUE); JointJS ports "often displayed as circles inside diagram elements" (Known, JJ-FEAT) |
| Glue (attachment kind) | Whether an end is fixed to one port (point glue) or moves to the nearest port as nodes move (dynamic glue) | DevExpress: point connection and "dynamic" connection to "a connection point that is nearest to the connector's point of origin", `BeginItemPointIndex = -1` for dynamic (Known, DX-CONN); Visio: point (static) and dynamic glue (Unverified, VISIO-GLUE) |
| Container (group) | A node that holds other nodes; moving it moves them; may collapse | yFiles grouping ("Moving a group node moves all of its children", folding, Known, YF-GRAPH); GoJS `Group` (Known, GOJS-INTRO); mxGraph `groupCells`, folding, `enterGroup` (Known, MX-MANUAL); JointJS embedding "collapsed or expanded as a group" (Known, JJ-FEAT); Visio containers and lists with `LockMembership` and categories (Unverified, VISIO-STRUCT); DevExpress "Containers and Lists" topic (existence Known, DX-CONN navigation) |
| Layer | A z-ordered set of items shown, hidden or locked together | mxGraph layers as children of the root, "Layer 0" (Known, MX-MANUAL); GoJS `Layer` (topic exists, GOJS-INTRO); Visio layers that control visibility, printing and locking (Unverified, VISIO-LAYER) |
| Router | Computes a connector's path from its endpoints, bends and obstacles | Telerik `AStarRouter` with `AvoidShapes`, `AvoidConnectionOverlap`, `WallOptimization` (Known, TEL-ROUTE); GoJS `Link.routing` with avoids-nodes routing (Known for AvoidsNodes, GOJS-INTRO); JointJS `manhattan`, `metro`, `orthogonal` routers (Unverified, JJ-ROUTE); MindFusion `RouteAllLinks` with a grid router of length, turn and crossing costs (Unverified, MF) |
| Connector geometry | Straight, orthogonal (right-angle), curved; line jumps where links cross | DevExpress "Straight Line, Curve, or Right angle" (Known, DX-CONN); GoJS Bezier curve (Known, GOJS-INTRO); JointJS `jumpover`, `rounded` connectors (Unverified, JJ-ROUTE); MSAGL spline, rectilinear, straight-line routing modes (Unverified, MSAGL) |
| Layout | Positions nodes (and routes edges) by an algorithm | GoJS layouts "after nodes or links are added or removed" (Known, GOJS-INTRO); DevExpress tree, mind-map, tip-over, Sugiyama methods (names Known, DX-LAYOUT); MSAGL as a library (Known, MSAGL); Telerik `OrgTreeRouter` tied to tree layouts (Known, TEL-ROUTE) |
| Transaction and undo | Edits grouped into one undoable step | GoJS `UndoManager` and "in a transaction" (Known, GOJS-INTRO); mxGraph `beginUpdate`/`endUpdate` nesting, "only one undo created" (Known, MX-MANUAL); GoDiagram `GoUndoManager` with compound edits (Unverified, GODIAGRAM); JointJS+ `CommandManager` (commercial plugin, Known, JJ-FEAT) |
| Tools (input modes) | Pointer behaviours: select, move, resize, draw a connector, edit bends, rubber band, pan | yFiles `GraphEditorInputMode` (Unverified detail, YF-EDIT); JointJS+ Halo, FreeTransform, Selection Region (Known, JJ-FEAT); GoJS tools (Inferred from the undo demo, GOJS-INTRO) |
| Guides and snapping | Grid snap; snap lines to other items' edges and centres while moving | yFiles `EdgeSnapLineProvider` (Unverified, YF-EDIT); JointJS "Snap to objects" (Known, JJ-FEAT); MindFusion "alignment to grid, interactive node alignment" (Unverified, MF) |
| Overview (minimap) | A small whole-diagram view with the viewport rectangle | JointJS+ Navigator (Known, JJ-FEAT) |

## 2. The canonical systems, one by one

### 2.1 GoJS and GoDiagram (Northwoods)

- **Model and view** (Known, GOJS-INTRO). Three model kinds: `Model` (parts
  without links or groups), `TreeModel` (a parent reference, "no separate
  link data array") and `GraphLinksModel` ("Arbitrary graphs with independent
  link data. Also allows Groups."). A `Part` (`Node`, `Link`, `Group`) is "a
  tree of visual GraphObjects" laid out by `Panel`s (Auto, Vertical,
  Horizontal, Spot, Table).
- **Ports** (Known, GOJS-PORTS). Any element of a node's visual tree becomes
  a port by setting `portId`; the empty string is the default port; links
  cannot have ports. `fromLinkable`/`toLinkable` allow drawing; `fromSpot`/
  `toSpot` set where links attach; a model names link ends' ports through
  `linkFromPortIdProperty`/`linkToPortIdProperty`; by default the user cannot
  draw more than one link in the same direction between the same two ports;
  `toMaxLinks` caps a port.
- **Routing and curves** (Known for avoids-nodes and Bezier, GOJS-INTRO):
  "Link.routing" can make the path "avoid crossing over nodes"; `curve:
  Bezier` "gives the Link a Bezier arc path". Normal and orthogonal routing
  exist (Unverified here).
- **Undo** (Known, GOJS-INTRO): the `UndoManager`; after the diagram exists,
  model changes must go through `Model.set` "in a transaction".
- **Scale** (Known, GOJS-PERF): "a few hundreds of nodes and links" need no
  effort; "thousands or tens of thousands" need care. Virtualization is an
  application pattern, not built in: the Virtualized Tree sample "contains
  123,456 total nodes" and "only constructs nodes and links that intersect
  with the viewport", keeping a separate model and laying out on
  `LayoutVertex`/`LayoutEdge`. Avoids-nodes routing "can be slow in very
  large graphs"; `LayeredDigraphLayout` "can be very slow on graphs with
  thousands of nodes and links".
- **Accessibility** (Known, GOJS-A11Y): keyboard focus off by default
  (Ctrl+Alt+Enter toggles it); arrows move focus among nearby objects in the
  current scope; Ctrl+arrows move from a node to a connected link or node;
  Enter enters a group, Escape leaves; focus is separate from selection; a
  Shift-driven virtual pointer lets the keyboard drag and draw links. The
  library "cannot know what is important to say and when": the application
  supplies the announcements through `CommandHandler.focusChanged` and a live
  element.
- **GoDiagram** (the Windows Forms product): `GoDocument` with an optional
  `GoUndoManager` that "observes and records document changes", compound
  edits, `MaximumEditCount`, warnings for changes outside a transaction
  (Unverified, GODIAGRAM).

### 2.2 yFiles (yWorks)

- **Model** (Known, YF-GRAPH): `IGraph`, "a comprehensive representation of
  the mathematical concept of a directed graph"; `INode`, `IEdge` (directed,
  source and target), `IPort` ("Edges connect to nodes via ports"; ports may
  also join edges to edges), `IBend`, `ILabel` for nodes and edges. Grouping:
  a parent node holds children, "The root of the hierarchy is `null`", moving
  a group moves its children, its minimum size is its children's bounds;
  folding collapses and expands groups.
- **Editing** (Unverified, YF-EDIT): `GraphEditorInputMode` is the default
  editor; orthogonal edge editing keeps edge paths orthogonal while the user
  edits; snap lines for nodes and orthogonal edges.
- **Scope** (Known, YF-INTRO): "an extensive class library that provides
  algorithms and components": visualization, "a meaningful arrangement of
  all elements", "interactive editing of the diagram", and persistence, each
  a chapter (Graph I/O; Automatic Layouts).

### 2.3 DevExpress Diagram (WinForms)

- **Connectors** (Known, DX-CONN): type `Straight Line`, `Curve` or `Right
  angle`; ends attach by `BeginItem`/`EndItem` (shapes) with
  `BeginItemPointIndex`/`EndItemPointIndex` (a connection point, or -1 for a
  dynamic connection), or by `BeginPoint`/`EndPoint` at free coordinates when
  no item is set; a straight connector with dynamic glue attaches to the
  shape's centre; connectors follow their shapes; `Points` holds turn points;
  `BeginArrow`/`EndArrow`; `Content` is the connector's text. The page does
  not say whether connectors route around shapes.
- **Persistence** (Known, DX-SAVE): XML, `SaveDocument`/`LoadDocument` to a
  file or stream; `CustomSaveDocument`/`CustomLoadDocument` events; custom
  item types registered for serialization.
- **Layouts** (names Known, behaviour Unverified, DX-LAYOUT): tree,
  mind-map tree, tip-over tree and Sugiyama methods on the control; the item
  set passed in should include the connectors.
- **Design intent** (Unverified, DX-BLOG): "Visio inspired" connectors and
  automatic routing.

### 2.4 Telerik RadDiagram (WinForms)

- **Routing** (Known, TEL-ROUTE): off by default (`RouteConnections`
  false). A grid of `RoutingGridSize` (40 units by default) trades speed and
  "wiggling" against finding paths in dense layouts. `AStarRouter` (A*
  search) with `AvoidShapes`, `WallOptimization` (fewer corners),
  `AvoidConnectionOverlap` (added R2 2021 SP1) with overlap and
  shape-crossing penalties; `InflatedRectRouter` "with the fewest bends";
  `OrgTreeRouter` for tree layouts. Routers are extended by overriding
  penalty methods. A support thread records that the router "doesn't always
  choose the most optimal route when a shape is moved" (Unverified).

### 2.5 Syncfusion and MindFusion (WinForms)

- **Syncfusion** (Unverified, SF): connectors with "automatic routing and
  line bridging" and decorators at both ends; layouts for org charts,
  hierarchical trees, flowcharts and mind maps (stated for the cross-platform
  SDK, not confirmed for WinForms); undo and redo through a history manager;
  the WinForms control saves "in both binary and SOAP formats".
- **MindFusion** (Unverified, MF): nested container nodes, table nodes,
  links with arrowheads, grid alignment, interactive alignment, zoom and pan;
  "more than 12 automatic graph layout algorithms"; `RouteAllLinks` routes
  each link "the shortest possible way between the nodes it links without
  crossing any other nodes", through a router chaining a pattern router and
  a grid router whose `LengthCost`, `TurnCost` and `CrossingCost` shape the
  route; undo records for text edits.

### 2.6 Microsoft Automatic Graph Layout (MSAGL)

Known (MSAGL README): "a .NET library and tool for graph layout and
viewing", in parts: the core layout engine (`AutomaticGraphLayout.dll`,
"contains the core layout functionality", usable alone when rendering
elsewhere), a drawing module (attributes and node, edge and graph classes),
a WPF control and a Windows Forms viewer (`GraphViewerGdi`), each with "Pan
and Zoom" and "Search for and focus on graph entities", and a JavaScript
version with "Limited interactivity". Unverified (search summaries): layouts
Sugiyama (layered), MDS, Ranking and a fast incremental force-directed
layout; edge routing modes spline, spline bundling, straight line, Sugiyama
splines, rectilinear and rectilinear to centre; MIT licence.

MSAGL is the clearest example of the split this study recommends (section
6.6): the layout engine is a separate library with its own geometry types,
and viewers consume its results (Inferred from the component list).

### 2.7 Visio

Unverified (VISIO-GLUE, VISIO-STRUCT, VISIO-LAYER): connectors glue to
connection points, vertices, handles and geometry; point (static) glue stays
on one point, dynamic glue walks to the nearest connection point as shapes
move; a page's `RouteStyle` sets dynamic connectors' default routing and
`ShapeRouteStyle` overrides it per connector; containers and lists (Visio
2010's structured diagrams) hold members, `LockMembership` freezes
membership, categories restrict who may join, lists hold ordered members;
layers control visibility, printing and locking. Visio is a document model
and an application, not a control; it is cited for its vocabulary (glue,
containers, lists, layers), which DevExpress echoes (Known for glue,
DX-CONN).

### 2.8 mxGraph and draw.io

Known (MX-MANUAL): `mxGraphModel` holds the structure and default visual
state; a cell is a vertex or an edge; every cell has a parent, the default
parent "an invisible cell with the same bounds as the graph"; child
positions are relative to the parent; `groupCells`/`ungroupCells`; folding
and drill-down (`enterGroup`, `exitGroup`, `home`). Layers are cells under
the root ("Layer 0" is "the default indirection"); z-order is child order.
Transactions: `beginUpdate`/`endUpdate` keep a counter; change events fire
when it returns to zero; nested sub-transactions make a compound change such
as a layout "only one undo"; "wrapping changes in try/finally". Changing a
cell directly "skips event notifications and undo records". `mxCodec`
encodes the model to XML. Tree, force-directed and hierarchical layouts run
on the client or the server. Unverified (MX-API): edge styles such as the
elbow connector; connection constraints; `mxUndoableEdit`.

### 2.9 JointJS

Known (JJ-BASICS, JJ-FEAT): `Graph` (model) and `Paper` (view); ports;
embedding into collapsible containers; link labels; the commercial JointJS+
adds the stencil palette, inspector, `PaperScroller` (zoom, pan, centre),
Navigator (minimap), Halo, FreeTransform, selection region, clipboard,
keyboard shortcuts, snap to objects, and `CommandManager` ("keeps track of
graph changes") for undo. Unverified (JJ-ROUTE): routers compute points and
connectors draw them; `manhattan` (orthogonal, obstacle-avoiding on a grid
`step` best equal to the paper's grid), `metro` (orthogonal and diagonal),
`orthogonal`; connectors `straight`, `rounded` (corner radius), `jumpover`
(arcs where links cross, not detected for smooth links), curves.

## 3. Side by side

### 3.1 Model

| | Ports | Groups | Layers | Model/view split | Edits |
|---|---|---|---|---|---|
| GoJS | any element by `portId` | `Group` | `Layer` | yes, separate models | transactions, `UndoManager` |
| yFiles | `IPort`, also on edges | parent nodes, folding | (not read) | `IGraph` vs component | (undo engine, not read) |
| DevExpress | connection points, point or dynamic glue | containers and lists | (not read) | items in the control | (not read) |
| mxGraph | constraints (Unverified) | parent cells, folding | cells under root | model vs view | nested `beginUpdate` |
| JointJS | ports | embedding | (not read) | `Graph` vs `Paper` | CommandManager (JointJS+) |
| Visio | connection points, glue | containers, lists | layers | document | application undo |

### 3.2 Routing

| | Straight | Orthogonal | Curved | Avoids nodes | Line jumps |
|---|---|---|---|---|---|
| DevExpress | yes | "Right angle" | "Curve" | not stated | (not read) |
| Telerik | yes | grid A* | (not read) | `AvoidShapes`, off by default | (not read) |
| GoJS | yes | yes (Unverified) | Bezier | avoids-nodes, slow at scale | JumpOver (Unverified) |
| JointJS | `straight` | `orthogonal`, `manhattan` | curves | `manhattan` | `jumpover` |
| MindFusion | yes | grid router with costs | (not read) | `RouteAllLinks` | (not read) |
| MSAGL | straight line | rectilinear | splines | (routing modes) | (not read) |

### 3.3 Where the systems agree (Inferred)

1. A separate model, saved and loaded, edited in transactions that make one
   undo step each; the view never owns the data.
2. Ports as named, ruled attachment places; connectors attached to a port,
   a node (dynamic) or a free point, with optional bends.
3. Three connector geometries (straight, orthogonal, curved); obstacle
   avoidance is a separate, optional and slower router.
4. Groups are nodes with children; moving moves the children; collapsing is
   common.
5. Automatic layout is a separate algorithm applied to a set of items, never
   continuous by default except in GoJS's "after nodes or links are added".
6. Accessibility is thin everywhere: GoJS is the only system read that
   documents keyboard navigation over a graph, and it leaves the speech to
   the application.

## 4. What GUI.Forms has today

All Known, read in the repository on 2026-10-18 (main at `1ee5b470` unless a
branch is named).

| Piece | Where | What it gives a diagram | What it lacks |
|---|---|---|---|
| `DrawingSurface` | `include/gui_forms/controls/drawing_surface/drawing_surface.hpp` | A control painted by an application callback (`std::function<void(Painter&, Rect, Rect)>`), optional hit testing, a settable semantic role | No model, no items, no parts beyond region, face and focus; the callback is outside the by-role paint path |
| `RasterCanvas` | `include/gui_forms/controls/raster_canvas/raster_canvas.hpp` | A retained viewport with `zoom` and `view_origin` over a bitmap: the zoom and pan bookkeeping pattern | Pixels, not items |
| `Painter` | `include/gui_forms/types/painter/painter.hpp` | Rectangles, rounded rectangles, gradients, shadows, `draw_line`, text, images; `save`, `restore`, `translate`, `clip_rect` | **No paths, polygons or curves, and no scale transform**: connectors with arrowheads and curves, and zoom, cannot be painted through it today |
| GUI.Drawing | `include/gui_forms/drawing/` (`graphics_path`, `graphics_recorder`, `matrix`) | `GraphicsPath` with `add_line`, `add_bezier`, `add_arc`; a recorder with `set_transform(Matrix)`, `fill_polygon`, `draw_path` | The System.Drawing-style library behind `RasterCanvas` and the compatibility facade; not the retained by-role path (Inferred from its users: `raster_canvas.cpp`, the images page) |
| By-role paint path | `include/gui_forms/theme/part_painting/part_painting.hpp` | `paint_part` by `PartRole`, states, family, marks, accent, content colour (W-7, PR stacked on #148) | No stroke-a-path role, no arrowhead marks, no shapes beyond rectangles and rounded rectangles |
| Anatomy | `include/gui_forms/anatomy/types/anatomy_types.hpp` | Parts, states, item systems with realized items (the virtualization pattern of `ObjectView`, `RibbonGallery`) | `maximum_control_parts = 32`, `maximum_item_systems = 4`, `maximum_custom_states = 16`; `PartGeometry` is a rectangle, so a connector's path is not describable |
| Hit testing | `Control::hit_test_local`, item-system controls' own hit tests (the ribbon, galleries) | Rectangle hit tests per control and per item | No path hit testing (distance to a polyline or curve); no spatial index |
| Layout transactions | `DockLayout` on branch `claude/dk-d1-layout` (`include/gui_forms/docking/dock_layout/dock_layout.hpp`) | A renderer-free model edited only through transactions of primitive edits, validated on a copy, applied all or nothing, one generation and one change event per transaction, `check` without committing, restrictions in layers | Not a graph; no inverse edits (undo) |
| Workspace transaction | `include/gui_forms/application/workspace_transaction/workspace_transaction.hpp` (main) | Update scopes over several windows | Not an edit model |
| XML | branch `claude/dk-d1-xml`: `include/gui_forms/xml/xml_reader/xml_reader.hpp`, `xml_writer` | A bounded reader of a stated subset: "The whole input is checked before a result is returned"; expected faults never throw; limits bound storage; a fuzz target (`fuzz/xml_reader_fuzz.cpp`) | A format for diagrams (section 6.5) |
| Stable identity | `StableId`, `DockPanelId` (branch), page IDs in `PageSet` (DK plan) | The house rule: items named by stable IDs, never by index | A diagram item ID type |
| Commands | `include/gui_forms/commands/` | `Command`, value commands | No undo stack: only `TextBox` and `MaskedTextBox` keep their own edit history (headers mention undo) |
| Semantics | `include/gui_forms/semantics/types/semantic_types.hpp` | Roles up to `split_button`, `menu_button`; node fields position in set, set size, has-popup | No graph roles (graphics document, object, symbol); no relation fields (connected to, flows to) |

## 5. A proposed model in GUI.Forms terms

Everything in this section is a proposal (Inferred), shaped by the house
style and the docking model.

### 5.1 Two layers, as docking (DOCK-PLAN)

- **`DiagramModel`**, renderer-free, in the core (`src/core/diagram/`), with
  no control, painter or host dependency, tested headless as `DockLayout` and
  `PageSet` are.
- **`DiagramView`**, a control that realizes the model in a viewport with
  zoom and pan, selection, tools, adornments and semantics; any number of
  views may show one model (Inferred from GoJS and JointJS's model/view
  split; a minimap is a second view).

### 5.2 The model's records

```text
DiagramItemId      stable string ID, one namespace per model (house style §1:
                   an identity, never an index)
DiagramNode        id, kind (a declared NodeKind), bounds (model units),
                   container (parent node or none), layer, z order within the
                   layer, label text, image, application data handle
DiagramPort        id (unique within its node), side or relative position,
                   direction (in, out, both), maximum connectors
DiagramConnector   id, source end and target end, each one of:
                     port (node id + port id), node (dynamic glue: nearest
                     port, or the boundary), free point;
                   geometry (straight, orthogonal, curved), bends (points the
                   user fixed), labels (text at a relative position),
                   arrowheads per end (declared kinds), layer
DiagramLayer       id, name, visible, locked, printable
```

`NodeKind` and arrowhead kinds are declared enumerations (house style §7:
templates and enums for declared choices; no universal callable framework);
a node's look comes from its kind and the theme, its content from text and an
image, never from an application paint callback (the atom ruling, SPEC
§12.7). Application data is an opaque handle the model stores and returns,
never interprets (Inferred: GoJS models hold arbitrary data objects; GUI.Forms
would hold a typed handle or key instead).

### 5.3 Transactions and undo hooks

Every change is a `DiagramTransaction` of primitive edits (add, remove,
move, resize, reparent, reconnect an end, set bends, set a property), as
`DockLayout`'s: validated on a copy (ports exist, limits hold, no cycles of
containment, locked layers refused), applied all or nothing, one generation
and one change event each (Known for the docking pattern, branch
`claude/dk-d1-layout`). In addition, each applied transaction returns its
**inverse** transaction, which the application may keep: undo is the
application's (or a small `DiagramHistory` component's) stack of inverses
(Inferred; GoJS, mxGraph and GoDiagram all make one transaction one undo
step). A tool's drag is one transaction at release, with a live preview that
is not a model change (Inferred: mxGraph previews during a move before
committing; GoJS's tools likewise, Unverified).

### 5.4 The view's anatomy

Within the anatomy limits (32 parts, 4 item systems):

| Item system | Parts (roles) | States |
|---|---|---|
| `diagram_node` | node_face (face), node_label (label), node_icon (icon), node_focus (focus) | hot, selected, focus_visible, disabled, drop_accepted, drop_rejected |
| `diagram_connector` | connector_path (**gap**: a stroked path), connector_label (label), connector_end_mark (glyph, **gap**: arrowhead marks) | hot, selected, focus_visible |
| `diagram_port` | port_mark (indicator) | hot, drop_accepted, drop_rejected |
| `diagram_handle` | handle (actuator), guide (keyline), rubber_band (selection) | hot, pressed |

The view's own parts: region, well (the canvas), grid (keyline),
scroll bars (composed), focus. Realized items are only those that intersect
the viewport (the item-system pattern), found through a spatial index over
model bounds (Inferred).

### 5.5 What is not proposed

- No graph-layout engine in GUI.Forms (section 6.6, question J1).
- No arbitrary application-drawn node bodies (the atom ruling); custom looks
  come from declared node kinds the theme draws.
- No file format promised beyond the model's own XML (matrix row 55; section
  6.5).
- No real-time collaboration or multi-user merge.

## 6. The hard parts

### 6.1 Painting connectors and zoom (vocabulary and engine gaps)

`Painter` has no path, polygon or curve primitive and no scale transform
(Known, section 4). A diagram needs: polylines with rounded corners,
quadratic or cubic curves, filled arrowheads, dashes, and drawing at a zoom
factor. Options: (1) extend `Painter` with a path primitive and a scale
transform (engine work for every renderer: Skia CPU, DIB, X11); (2) draw at
zoom by scaling coordinates in the control and clamp text sizes, as
`RasterCanvas` does for pixels (Inferred: no painter change, but text and
line widths at small zoom need rules); (3) both. The vocabulary also lacks a
"stroked path" part role and arrowhead marks (SPEC §8.5 marks are arrows and
chevrons for glyph boxes, not line ends). Questions K1 to K3.

### 6.2 Hit testing a line

Rectangle hit tests do not fit a diagonal or curved connector: a connector
is hit within a tolerance of its path (Inferred; every system read lets the
user click a link). With many items the view needs a spatial index (a
uniform grid or an R-tree) over node bounds and connector segment bounds,
updated per transaction (Inferred).

### 6.3 Routing

Straight and simple orthogonal geometry (elbows between ports, honouring
user bends) are local computations (Inferred). Obstacle-avoiding orthogonal
routing is a search on a grid or a visibility graph, sensitive to grid size
(Known for Telerik's `RoutingGridSize` trade-off, TEL-ROUTE) and slow at
scale (Known for GoJS's avoids-nodes, GOJS-PERF); Telerik ships it off by
default (Known). Proposal: straight and orthogonal in the first phases;
avoidance as a later, optional router run on demand or on the edited
connectors only (question C2).

### 6.4 Manipulation

Selection (click, Ctrl or Shift toggles, rubber band), moving with snapping
to a grid and to other items' edges and centres (alignment guides), resizing
by eight handles, reconnecting an end by dragging it to another port,
drawing a connector from a port, editing bends, keyboard nudging. Each drag
is one transaction (section 5.3). These are the canonical tools (Known in
part: JointJS+'s selection region, FreeTransform, snap to objects, JJ-FEAT;
yFiles' snap lines, Unverified).

### 6.5 Persistence

The model writes and reads its own XML through the core XML reader and
writer (branch `claude/dk-d1-xml`), with a format version and an upgrade
hook as the docking layout has (Known for `DockLayoutUpgradeHook`, branch
`claude/dk-d1-layout`); stable IDs make diffs readable (Inferred). Every
vendor read has its own format (DevExpress XML, Known; Syncfusion binary and
SOAP, Unverified; mxGraph XML through `mxCodec`, Known; JointJS JSON,
Unverified). Import of Visio, draw.io or GraphML is application work or a
later phase (question G2).

### 6.6 Automatic layout: toolkit or application?

The systems split three ways: built into the control (DevExpress' layout
methods, GoJS's layouts, MindFusion's "more than 12"), a separate library
(MSAGL's core engine; yFiles' layout chapter is a separate part of a larger
library) or absent (JointJS core, with layouts as a feature page of its own;
Visio as an application). A layered (Sugiyama) layout is a substantial
algorithm whose quality depends on crossing minimization and coordinate
assignment, and it is slow at thousands of nodes (Known for GoJS's
`LayeredDigraphLayout`, GOJS-PERF). Recommendation (Inferred): no layout
engine in GUI.Forms; the model accepts positions from any source in one
transaction (an application, MSAGL used through its core engine, or a later
optional module), and a grid or simple tree arrangement may be offered as a
sample. This keeps matrix row 55's "No graph-layout engine ... promised".
Question J1.

### 6.7 Accessibility

What exists elsewhere: ARIA's graphics module defines `graphics-document`
("A type of document in which the visual appearance or layout of content
conveys meaning"), `graphics-object` and `graphics-symbol`, recommends
hierarchical navigation of nested objects, and shows a connector labelled by
the objects it joins (`aria-labelledby`); it gives no general rule for
relations (Known, ARIA-GFX). GoJS gives keyboard focus over nodes, links and
groups and leaves the words to the application (Known, GOJS-A11Y). Kendo's
diagram uses `graphics-document` with shapes as `graphics-symbol` and
`aria-activedescendant` (Unverified, KENDO-ARIA). Research reports that trees
capture containment but not connections and adjacency lists the reverse
(Unverified, BENTHIC), and that blind users can examine, navigate and create
graphs with a suited tool (Unverified, GSK).

Proposal (Inferred): publish the diagram as a group (or a new graphics role,
question I1) whose children are its nodes in a stated reading order (layer,
then top-to-bottom, left-to-right, or an application order), each node
listing its connectors as children or as relations (question I2), with
positions in set; keyboard: arrows move focus spatially, a modifier follows
connectors (GoJS's model), Enter enters a container, Escape leaves, a
keyboard move and connect command set (no virtual pointer). Host mappings
(MSAA, UIA, AX, ATK) of any new role or relation are stage A's; none of the
four host APIs was checked for a graph pattern in this study (Unverified).

### 6.8 Scale

Proposed bounds for a first gate, to be measured, not promised (house
style: no "fast" without a workload): models of 10,000 nodes and 15,000
connectors held and edited; views realizing only visible items; a resize or
pan that measures no text (as the ribbon's caches); routing and layout
outside the interactive path for large graphs (Known for GoJS's guidance,
GOJS-PERF; numbers Inferred).

## 7. Phased plan

Each phase ends with tests in `tests/` on every CI platform and its own PR
(the DK and RB pattern).

- **Phase 0. Vocabulary and engine.** Owner decisions K1 to K3: a path
  primitive or not, a scale transform or not, the stroked-path part role, the
  arrowhead marks, the graph semantic role and relation fields (stage A for
  host mappings). Tests: painting and schema of the new atoms.
- **Phase 1. `DiagramModel`.** Nodes, ports, connectors (straight geometry,
  bends), containers, layers; transactions with validation, generation,
  change events and inverses; XML write and restore with version and upgrade
  hook. Headless tests: every edit's validation and inverse, containment
  rules, round trip, fuzzed restore.
- **Phase 2. `DiagramView`, reading.** Viewport with zoom and pan (wheel,
  keyboard, scroll bars), realized items through a spatial index, painting
  by role, path hit testing, semantics and keyboard focus navigation.
  Tests: realization bound at a 10,000-node model, hit tests on lines,
  semantics snapshot.
- **Phase 3. Editing.** Selection and rubber band, move, resize, snapping to
  grid and guides, drawing and reconnecting connectors with port rules,
  bends, keyboard nudge and connect, one transaction per gesture, an undo
  history component. Tests: each tool's transaction and its inverse.
- **Phase 4. Orthogonal routing and labels.** Elbow routing between ports,
  user bends kept orthogonal, connector labels; then optional obstacle
  avoidance on demand. Tests: route goldens.
- **Phase 5. Curves, containers' collapse, minimap, print and image
  export.** Tests: goldens and collapse transactions.
- **Phase 6 (optional, owner's call).** Interop: import or export of an
  external format; a sample layout module.

Dependencies: the XML reader (DK, branch), the docking transaction pattern
(DK), T4 for node and connector looks, T2 for motion (selection and handle
animation, if any), stage A for host mappings.

## 8. Open owner decisions, as interview questions

House form: short numbered options with a recommendation each. IDs are
proposed as `DG-` plus a letter and number.

### A. Scope

**A1. What is the first Diagram for?**
1. Flowcharts and simple node-link editors (process, state, dependency).
2. A general graph editor (any graph, large, with layout).
3. A drawing tool with connectors (Visio-like shapes and pages).
Recommendation: 1; it covers the common Windows Forms need and keeps the
first gates measurable.

**A2. Model and view separate?**
1. A renderer-free `DiagramModel` in the core plus a `DiagramView` control
   (several views per model).
2. One control that holds its items.
Recommendation: 1, as `DockLayout`/`DockSite` and every system read.

**A3. Who owns node looks?**
1. Declared node kinds (rectangle, rounded, ellipse, diamond, parallelogram,
   document, cylinder, ...) drawn by the theme by role, with text and image
   content.
2. Application-drawn node bodies through a callback.
3. Both, the callback as an escape hatch.
Recommendation: 1 (the atom ruling, SPEC §12.7); kinds can be added as
vocabulary.

**A4. Verify the Unverified sources first?**
1. Yes: a second pass opening the primary pages of yFiles, Syncfusion,
   MindFusion, GoDiagram, Visio, mxGraph and JointJS before the plan.
2. No: the plan rests on the Known material.
Recommendation: 1 for the routing and accessibility facts only.

### B. Model

**B1. Ports.**
1. Named ports on nodes with side or position, direction and a link limit;
   ends may also attach to a node (dynamic, nearest port or boundary) or a
   free point.
2. Node-to-node only (no ports).
Recommendation: 1 (DevExpress' point and dynamic glue; GoJS ports).

**B2. Containers.**
1. Containers are nodes with children; moving moves children; collapse in a
   later phase.
2. No containers in the first model.
Recommendation: 1.

**B3. Layers.**
1. Named layers with visible, locked and printable flags, z order within a
   layer.
2. One z order, no layers.
Recommendation: 1 (mxGraph, Visio).

**B4. Application data.**
1. An opaque typed handle per item the model stores and returns.
2. A string-keyed property bag per item.
Recommendation: 1 (house style: typed, no general `std::any`).

### C. Routing and geometry

**C1. First geometries.** 1. Straight and orthogonal. 2. Straight only. 3.
Straight, orthogonal and curved. Recommendation: 1; curves in phase 5.

**C2. Obstacle avoidance.**
1. A later optional router, run on demand or on edited connectors only.
2. Always on.
3. Never in the toolkit.
Recommendation: 1 (Telerik's default off; GoJS's cost at scale).

**C3. Line jumps where connectors cross.** 1. Later, optional. 2. Never.
Recommendation: 1.

### D. Selection and manipulation

**D1. Tools in the first editing phase.** 1. Select, rubber band, move,
resize, draw and reconnect connectors, bends, nudge. 2. Select and move
only. Recommendation: 1.

**D2. Snapping.** 1. Grid and alignment guides (edges and centres of nearby
items). 2. Grid only. 3. None. Recommendation: 1.

**D3. Rotation of nodes.** 1. Not in the first phases. 2. Yes. Recommendation: 1.

### E. Zoom and pan

**E1. Zoom model.** 1. Continuous zoom with wheel and keyboard, steps for
the keyboard, fit to content and selection. 2. Fixed steps only.
Recommendation: 1.

**E2. Text at small zoom.** 1. Text below a readable size is replaced by its
outline (greeking). 2. Text always drawn. Recommendation: 1 (Inferred cost
and legibility).

### F. Undo

**F1. Undo hooks.**
1. Each transaction returns its inverse; a small optional `DiagramHistory`
   keeps the stack; the application may use its own.
2. A full undo manager built into the view.
3. No undo support.
Recommendation: 1.

### G. Persistence

**G1. Format.** 1. The model's own XML through the core reader and writer,
versioned, with an upgrade hook. 2. JSON. 3. No persistence in the toolkit.
Recommendation: 1 (the XML reader exists; DK's pattern).

**G2. Interop.** 1. None in the toolkit; applications convert. 2. An
importer for one external format later (GraphML, draw.io or Visio VSDX).
Recommendation: 1 now; revisit after phase 5.

### H. Scale

**H1. Gate workload.** 1. 10,000 nodes and 15,000 connectors held and
edited, views realizing only visible items, measured. 2. 1,000 nodes.
Recommendation: 1, as a measurement, not a promise.

### I. Accessibility

**I1. Semantic role.** 1. Admit graphics roles (a graphics document and
graphics objects) to the vocabulary, mapped by stage A. 2. Use `group` and
`list_item` (no new roles). Recommendation: 1 (ARIA-GFX), mapping with
stage A.

**I2. Connections to assistive technology.**
1. Each node lists its connectors (and their other ends) as children with
   positions in set.
2. A relation field (connected to, flows to) on nodes.
3. Both.
Recommendation: 3 if stage A can map a relation; otherwise 1.

**I3. Keyboard model.**
1. Arrows move focus spatially; a modifier with arrows follows connectors;
   Enter enters a container, Escape leaves; commands to move and connect
   by keyboard (no virtual pointer).
2. GoJS's virtual pointer as well.
Recommendation: 1.

### J. Layout

**J1. Automatic layout.**
1. Not in GUI.Forms: positions come from the application or an external
   library in one transaction; a grid arrangement as a sample.
2. A tree and a layered layout in an optional module.
3. Built into the view.
Recommendation: 1 (matrix row 55; MSAGL's separation).

### K. Vocabulary and engine (each a yes or no; SPEC §12.7)

**K1. A path primitive on `Painter`** (polylines, quadratic and cubic
curves, fill and stroke with dashes) for every renderer. Recommend yes.

**K2. A scale transform on `Painter`** for zoom, with text and line-width
rules. Recommend yes, after a measured spike.

**K3. New atoms:** a stroked-path part role (`connector`), arrowhead marks
(open, filled, diamond, circle), node shape marks or kinds, a
`drop_accepted` use on ports (exists). Recommend yes, admitted together in
phase 0.

### L. Fit

**L1. Where is it built?** 1. A ministage (study done, interview, plan)
like DK and RB, then W builds it. 2. Straight into W. Recommendation: 1.

**L2. Order.** 1. After DK's models and the XML reader land on main. 2.
Now. Recommendation: 1.

### Questions for the owner's taste (mockups)

- **T1. Connector line.** Hairline (one unit) or 1.5-unit stroke; solid ink
  or a muted graphite; how selected connectors show (thicker, accent, halo).
- **T2. Node faces.** Flat cards on the canvas, softly raised (the house
  card), or outlined only; corner radius.
- **T3. Ports.** Always visible small circles, visible only on hover or
  while connecting, or invisible with a highlight when a connector end is
  near.
- **T4. Selection.** Handles as small squares, circles or the house grip
  dots; rubber band as a tinted fill or a dashed outline.
- **T5. Canvas.** Plain, dot grid or line grid; whether the grid shows only
  while snapping.
- **T6. Guides.** Alignment guides as thin accent lines, dashed lines, or
  small distance labels too.

### Questions a mockup answers better than words

T1 to T6 above, shown as one flowchart in two or three variants each, as the
docking look studies were.

### Interview IDs used

None yet: this study precedes the interview. It cites the owner's matrix row
55 (2026-10-14) and the W brief's "Advanced candidates" list.
