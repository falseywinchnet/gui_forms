"""Summarizes one observation folder (probes.csv, transitions.csv, states.json)
into transitions.md: each state boundary with its exact pixel, in physical and
logical pixels, and what changed group by group."""
import csv
import json
import sys
from pathlib import Path


def items(group):
    return {} if group.get("collapsed") else dict(
        (entry.rsplit(":", 1)[0], entry.rsplit(":", 1)[1]) for entry in group.get("items", []))


def describe_item(size):
    width, height = (int(v) for v in size.split("x"))
    return f"{width}x{height}"


def change(before, after):
    if before.get("hidden") and after.get("hidden"):
        return "none"
    if after.get("hidden"):
        return "command area hidden"
    if before.get("hidden"):
        return "command area shown"
    old = {g["name"]: g for g in before.get("groups", [])}
    out = []
    for g in after.get("groups", []):
        o = old.get(g["name"])
        if o is None:
            out.append(f"{g['name']} appears")
            continue
        if bool(o.get("collapsed")) != bool(g.get("collapsed")):
            out.append(f"{g['name']} {'collapses to its button' if g.get('collapsed') else 'expands from its button'}")
            continue
        a, b = items(o), items(g)
        diffs = []
        for name in b:
            if name in a and a[name] != b[name]:
                diffs.append(f"{name} {describe_item(a[name])}->{describe_item(b[name])}")
        for name in a:
            if name not in b:
                diffs.append(f"{name} hidden")
        for name in b:
            if name not in a:
                diffs.append(f"{name} shown")
        if diffs:
            out.append(f"{g['name']}: " + ", ".join(diffs))
    if bool(before.get("scrolling")) != bool(after.get("scrolling")):
        out.append("scroll buttons " + ("appear" if after.get("scrolling") else "disappear"))
    return "; ".join(out) if out else "layout change with no item size change"


def main(folder, dpi):
    folder = Path(folder)
    scale = dpi / 96.0
    states = json.loads((folder / "states.json").read_text(encoding="utf-8-sig"))
    rows = list(csv.DictReader(open(folder / "transitions.csv", encoding="utf-8-sig")))
    lines = ["| tab | ladder | direction | at (fixed) | last px of old state | first px of new state | logical | from | to | what changed |",
             "|---|---|---|---|---|---|---|---|---|---|"]
    for r in rows:
        logical = int(r["last_px_of_from"]) / scale
        what = change(states[r["from"]]["detail"], states[r["to"]]["detail"])
        lines.append(f"| {r['tab']} | {r['ladder']} | {r['direction']} | {r['fixed']} | {r['last_px_of_from']} | "
                     f"{r['first_px_of_to']} | {logical:g} | {r['from']} | {r['to']} | {what} |")
    lines.append("")
    lines.append("States (signature: group{item:widthxheight} in physical pixels):")
    lines.append("")
    for key, value in states.items():
        lines.append(f"- **{key}** ({value['first_seen']}): `{value['signature']}`")
    (folder / "transitions.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines[:len(rows) + 2]))


if __name__ == "__main__":
    main(sys.argv[1], int(sys.argv[2]))
