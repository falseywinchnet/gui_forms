#!/usr/bin/env python3
"""List the [Obsolete] types and members of the Windows Forms reference assemblies.

Reads `Microsoft.WindowsDesktop.App.Ref` packages (.nupkg, a zip) as data: the
ECMA-335 metadata of `ref/<tfm>/System.Windows.Forms.dll` and
`System.Windows.Forms.Primitives.dll` is parsed here; nothing in the package is
executed. Writes one TSV row per obsolete type, method, property, event or
field: the package version, the owner type (nested types joined with '.', as
`PublicAPI.Shipped.txt` writes them), the member name (a constructor is the
type's own name, as PublicAPI writes it), the kind, the parameter count of a
method, the diagnostic ID and the message.

    python3 -I tools/winforms_catalogue/read_obsolete.py \\
        --package ref-8.0.32.nupkg --package ref-9.0.21.nupkg \\
        --output obsolete.tsv
"""

from __future__ import annotations

import argparse
import csv
import re
import struct
import zipfile
from pathlib import Path


ASSEMBLIES = ("System.Windows.Forms.dll", "System.Windows.Forms.Primitives.dll")

# Table numbers (ECMA-335 §II.22).
MODULE, TYPE_REF, TYPE_DEF, FIELD_PTR, FIELD, METHOD_PTR, METHOD_DEF = 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
PARAM_PTR, PARAM, INTERFACE_IMPL, MEMBER_REF, CONSTANT, CUSTOM_ATTRIBUTE = 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
FIELD_MARSHAL, DECL_SECURITY, CLASS_LAYOUT, FIELD_LAYOUT, STAND_ALONE_SIG = 0x0D, 0x0E, 0x0F, 0x10, 0x11
EVENT_MAP, EVENT_PTR, EVENT, PROPERTY_MAP, PROPERTY_PTR, PROPERTY = 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
METHOD_SEMANTICS, METHOD_IMPL, MODULE_REF, TYPE_SPEC, IMPL_MAP, FIELD_RVA = 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D
ENC_LOG, ENC_MAP, ASSEMBLY, ASSEMBLY_PROCESSOR, ASSEMBLY_OS, ASSEMBLY_REF = 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23
ASSEMBLY_REF_PROCESSOR, ASSEMBLY_REF_OS, FILE, EXPORTED_TYPE, MANIFEST_RESOURCE = 0x24, 0x25, 0x26, 0x27, 0x28
NESTED_CLASS, GENERIC_PARAM, METHOD_SPEC, GENERIC_PARAM_CONSTRAINT = 0x29, 0x2A, 0x2B, 0x2C

# Coded index kinds: the tables each tag selects (None for an unused tag).
CODED = {
    "TypeDefOrRef": (TYPE_DEF, TYPE_REF, TYPE_SPEC),
    "HasConstant": (FIELD, PARAM, PROPERTY),
    "HasCustomAttribute": (METHOD_DEF, FIELD, TYPE_REF, TYPE_DEF, PARAM, INTERFACE_IMPL, MEMBER_REF, MODULE,
                           DECL_SECURITY, PROPERTY, EVENT, STAND_ALONE_SIG, MODULE_REF, TYPE_SPEC, ASSEMBLY,
                           ASSEMBLY_REF, FILE, EXPORTED_TYPE, MANIFEST_RESOURCE, GENERIC_PARAM,
                           GENERIC_PARAM_CONSTRAINT, METHOD_SPEC),
    "HasFieldMarshal": (FIELD, PARAM),
    "HasDeclSecurity": (TYPE_DEF, METHOD_DEF, ASSEMBLY),
    "MemberRefParent": (TYPE_DEF, TYPE_REF, MODULE_REF, METHOD_DEF, TYPE_SPEC),
    "HasSemantics": (EVENT, PROPERTY),
    "MethodDefOrRef": (METHOD_DEF, MEMBER_REF),
    "MemberForwarded": (FIELD, METHOD_DEF),
    "Implementation": (FILE, ASSEMBLY_REF, EXPORTED_TYPE),
    "CustomAttributeType": (None, None, METHOD_DEF, MEMBER_REF, None),
    "ResolutionScope": (MODULE, MODULE_REF, ASSEMBLY_REF, TYPE_REF),
    "TypeOrMethodDef": (TYPE_DEF, METHOD_DEF),
}

# Column kinds: "2" and "4" are fixed integers, "S" "G" "B" heap indexes,
# an int is a simple index into that table, a str a coded index kind.
SCHEMA = {
    MODULE: ("2", "S", "G", "G", "G"),
    TYPE_REF: ("ResolutionScope", "S", "S"),
    TYPE_DEF: ("4", "S", "S", "TypeDefOrRef", FIELD, METHOD_DEF),
    FIELD_PTR: (FIELD,),
    FIELD: ("2", "S", "B"),
    METHOD_PTR: (METHOD_DEF,),
    METHOD_DEF: ("4", "2", "2", "S", "B", PARAM),
    PARAM_PTR: (PARAM,),
    PARAM: ("2", "2", "S"),
    INTERFACE_IMPL: (TYPE_DEF, "TypeDefOrRef"),
    MEMBER_REF: ("MemberRefParent", "S", "B"),
    CONSTANT: ("2", "HasConstant", "B"),
    CUSTOM_ATTRIBUTE: ("HasCustomAttribute", "CustomAttributeType", "B"),
    FIELD_MARSHAL: ("HasFieldMarshal", "B"),
    DECL_SECURITY: ("2", "HasDeclSecurity", "B"),
    CLASS_LAYOUT: ("2", "4", TYPE_DEF),
    FIELD_LAYOUT: ("4", FIELD),
    STAND_ALONE_SIG: ("B",),
    EVENT_MAP: (TYPE_DEF, EVENT),
    EVENT_PTR: (EVENT,),
    EVENT: ("2", "S", "TypeDefOrRef"),
    PROPERTY_MAP: (TYPE_DEF, PROPERTY),
    PROPERTY_PTR: (PROPERTY,),
    PROPERTY: ("2", "S", "B"),
    METHOD_SEMANTICS: ("2", METHOD_DEF, "HasSemantics"),
    METHOD_IMPL: (TYPE_DEF, "MethodDefOrRef", "MethodDefOrRef"),
    MODULE_REF: ("S",),
    TYPE_SPEC: ("B",),
    IMPL_MAP: ("2", "MemberForwarded", "S", MODULE_REF),
    FIELD_RVA: ("4", FIELD),
    ENC_LOG: ("4", "4"),
    ENC_MAP: ("4",),
    ASSEMBLY: ("4", "2", "2", "2", "2", "4", "B", "S", "S"),
    ASSEMBLY_PROCESSOR: ("4",),
    ASSEMBLY_OS: ("4", "4", "4"),
    ASSEMBLY_REF: ("2", "2", "2", "2", "4", "B", "S", "S", "B"),
    ASSEMBLY_REF_PROCESSOR: ("4", ASSEMBLY_REF),
    ASSEMBLY_REF_OS: ("4", "4", "4", ASSEMBLY_REF),
    FILE: ("4", "S", "B"),
    EXPORTED_TYPE: ("4", "4", "S", "S", "Implementation"),
    MANIFEST_RESOURCE: ("4", "4", "S", "Implementation"),
    NESTED_CLASS: (TYPE_DEF, TYPE_DEF),
    GENERIC_PARAM: ("2", "2", "TypeOrMethodDef", "S"),
    METHOD_SPEC: ("MethodDefOrRef", "B"),
    GENERIC_PARAM_CONSTRAINT: (GENERIC_PARAM, "TypeDefOrRef"),
}


class Metadata:
    """The tables and heaps of one assembly's metadata, decoded eagerly."""

    def __init__(self, image: bytes) -> None:
        self.image = image
        root = self._metadata_root()
        streams = self._streams(root)
        self.strings = streams["#Strings"]
        self.blob = streams["#Blob"]
        self.tables = self._tables(streams["#~"])

    # -- PE and stream layout ------------------------------------------
    def _offset(self, rva: int) -> int:
        for virtual, size, raw in self.sections:
            if virtual <= rva < virtual + size:
                return rva - virtual + raw
        raise ValueError(f"RVA {rva:#x} is outside every section")

    def _metadata_root(self) -> int:
        image = self.image
        pe = struct.unpack_from("<I", image, 0x3C)[0]
        if image[pe:pe + 4] != b"PE\0\0":
            raise ValueError("not a PE image")
        section_count = struct.unpack_from("<H", image, pe + 6)[0]
        optional_size = struct.unpack_from("<H", image, pe + 20)[0]
        optional = pe + 24
        magic = struct.unpack_from("<H", image, optional)[0]
        directories = optional + (96 if magic == 0x10B else 112)
        cli_rva = struct.unpack_from("<I", image, directories + 14 * 8)[0]
        self.sections = []
        table = optional + optional_size
        for index in range(section_count):
            entry = table + index * 40
            virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from("<IIII", image, entry + 8)
            self.sections.append((virtual_address, max(virtual_size, raw_size), raw_pointer))
        cli = self._offset(cli_rva)
        metadata_rva = struct.unpack_from("<I", image, cli + 8)[0]
        return self._offset(metadata_rva)

    def _streams(self, root: int) -> dict[str, bytes]:
        image = self.image
        if struct.unpack_from("<I", image, root)[0] != 0x424A5342:
            raise ValueError("metadata signature missing")
        version_length = struct.unpack_from("<I", image, root + 12)[0]
        position = root + 16 + version_length
        count = struct.unpack_from("<H", image, position + 2)[0]
        position += 4
        streams = {}
        for _ in range(count):
            offset, size = struct.unpack_from("<II", image, position)
            position += 8
            end = image.index(b"\0", position)
            name = image[position:end].decode("ascii")
            position = (end + 4) & ~3
            streams[name] = image[root + offset:root + offset + size]
        return streams

    # -- tables -----------------------------------------------------------
    def _tables(self, stream: bytes) -> dict[int, list[tuple]]:
        heap_sizes = stream[6]
        valid = struct.unpack_from("<Q", stream, 8)[0]
        position = 24
        rows = {}
        for table in range(64):
            if valid >> table & 1:
                rows[table] = struct.unpack_from("<I", stream, position)[0]
                position += 4
        string_size = 4 if heap_sizes & 1 else 2
        guid_size = 4 if heap_sizes & 2 else 2
        blob_size = 4 if heap_sizes & 4 else 2

        def column_size(kind: object) -> int:
            if kind == "2":
                return 2
            if kind == "4":
                return 4
            if kind == "S":
                return string_size
            if kind == "G":
                return guid_size
            if kind == "B":
                return blob_size
            if isinstance(kind, int):
                return 2 if rows.get(kind, 0) < 0x10000 else 4
            targets = CODED[kind]
            tag_bits = (len(targets) - 1).bit_length()
            largest = max(rows.get(target, 0) for target in targets if target is not None)
            return 2 if largest < (1 << (16 - tag_bits)) else 4

        tables = {}
        for table in sorted(rows):
            if table not in SCHEMA:
                raise ValueError(f"metadata table {table:#x} has no schema")
            sizes = [column_size(kind) for kind in SCHEMA[table]]
            decoded = []
            for _ in range(rows[table]):
                values = []
                for size in sizes:
                    values.append(struct.unpack_from("<H" if size == 2 else "<I", stream, position)[0])
                    position += size
                decoded.append(tuple(values))
            tables[table] = decoded
        return tables

    def string(self, index: int) -> str:
        end = self.strings.index(b"\0", index)
        return self.strings[index:end].decode("utf-8")

    def blob_at(self, index: int) -> bytes:
        length, position = compressed(self.blob, index)
        return self.blob[position:position + length]


def compressed(data: bytes, position: int) -> tuple[int, int]:
    """Decodes an ECMA-335 compressed unsigned integer; returns (value, next)."""
    first = data[position]
    if first & 0x80 == 0:
        return first, position + 1
    if first & 0xC0 == 0x80:
        return ((first & 0x3F) << 8) | data[position + 1], position + 2
    value = ((first & 0x1F) << 24) | (data[position + 1] << 16) | (data[position + 2] << 8) | data[position + 3]
    return value, position + 4


def decode_coded(kind: str, value: int) -> tuple[int | None, int]:
    targets = CODED[kind]
    tag_bits = (len(targets) - 1).bit_length()
    return targets[value & ((1 << tag_bits) - 1)], value >> tag_bits


def ser_string(data: bytes, position: int) -> tuple[str | None, int]:
    if data[position] == 0xFF:
        return None, position + 1
    length, position = compressed(data, position)
    return data[position:position + length].decode("utf-8"), position + length


def parameter_count(signature: bytes) -> int:
    flags = signature[0]
    position = 1
    if flags & 0x10:
        _, position = compressed(signature, position)
    count, _ = compressed(signature, position)
    return count


def owned_ranges(owners: list[tuple], column: int, total: int) -> list[tuple[int, int]]:
    """Each owner row's [first, end) range of a list column (1-based rows)."""
    ranges = []
    for index, row in enumerate(owners):
        first = row[column]
        end = owners[index + 1][column] if index + 1 < len(owners) else total + 1
        ranges.append((first, end))
    return ranges


def obsolete_rows(metadata: Metadata, version: str) -> list[dict[str, str]]:
    tables = metadata.tables
    type_defs = tables.get(TYPE_DEF, [])
    # Full type names, nested types joined with '.'.
    enclosing = {}
    for nested, outer in tables.get(NESTED_CLASS, []):
        enclosing[nested] = outer
    names: dict[int, str] = {}

    def type_name(row: int) -> str:
        if row in names:
            return names[row]
        flags, name, namespace = type_defs[row - 1][0], type_defs[row - 1][1], type_defs[row - 1][2]
        leaf = metadata.string(name)
        leaf = re.sub(r"`\d+$", "", leaf)
        if row in enclosing:
            full = type_name(enclosing[row]) + "." + leaf
        else:
            space = metadata.string(namespace)
            full = f"{space}.{leaf}" if space else leaf
        names[row] = full
        return full

    method_owner = {}
    for index, (first, end) in enumerate(owned_ranges(type_defs, 5, len(tables.get(METHOD_DEF, []))), 1):
        for method in range(first, end):
            method_owner[method] = index
    field_owner = {}
    for index, (first, end) in enumerate(owned_ranges(type_defs, 4, len(tables.get(FIELD, []))), 1):
        for field in range(first, end):
            field_owner[field] = index
    property_owner = {}
    property_maps = tables.get(PROPERTY_MAP, [])
    for (parent, _), (first, end) in zip(property_maps, owned_ranges(property_maps, 1, len(tables.get(PROPERTY, [])))):
        for prop in range(first, end):
            property_owner[prop] = parent
    event_owner = {}
    event_maps = tables.get(EVENT_MAP, [])
    for (parent, _), (first, end) in zip(event_maps, owned_ranges(event_maps, 1, len(tables.get(EVENT, [])))):
        for event in range(first, end):
            event_owner[event] = parent

    type_refs = tables.get(TYPE_REF, [])
    member_refs = tables.get(MEMBER_REF, [])
    method_defs = tables.get(METHOD_DEF, [])

    def attribute_type(kind: int | None, row: int) -> str:
        if kind == MEMBER_REF:
            parent_kind, parent_row = decode_coded("MemberRefParent", member_refs[row - 1][0])
            if parent_kind == TYPE_REF:
                _, name, namespace = type_refs[parent_row - 1]
                return f"{metadata.string(namespace)}.{metadata.string(name)}"
            if parent_kind == TYPE_DEF:
                return type_name(parent_row)
        if kind == METHOD_DEF:
            return type_name(method_owner[row])
        return ""

    rows = []
    for parent, attribute, value in tables.get(CUSTOM_ATTRIBUTE, []):
        ctor_kind, ctor_row = decode_coded("CustomAttributeType", attribute)
        if attribute_type(ctor_kind, ctor_row) != "System.ObsoleteAttribute":
            continue
        ctor_signature = metadata.blob_at(member_refs[ctor_row - 1][2] if ctor_kind == MEMBER_REF
                                          else method_defs[ctor_row - 1][4])
        fixed = parameter_count(ctor_signature)
        data = metadata.blob_at(value)
        message, position = (None, 2)
        if fixed >= 1:
            message, position = ser_string(data, 2)
        if fixed >= 2:
            position += 1
        diagnostic = ""
        if position + 2 <= len(data):
            named = struct.unpack_from("<H", data, position)[0]
            position += 2
            for _ in range(named):
                position += 1  # field or property
                element = data[position]
                position += 1
                field_name, position = ser_string(data, position)
                if element != 0x0E:
                    break
                text, position = ser_string(data, position)
                if field_name == "DiagnosticId" and text:
                    diagnostic = text
        target_kind, target = decode_coded("HasCustomAttribute", parent)
        kind, owner, member, count = "", "", "", ""
        if target_kind == TYPE_DEF:
            kind, owner = "type", type_name(target)
        elif target_kind == METHOD_DEF:
            row = method_defs[target - 1]
            name = metadata.string(row[3])
            owner = type_name(method_owner[target])
            if name in (".ctor", ".cctor"):
                name = owner.rsplit(".", 1)[-1]
            kind, member, count = "method", name, str(parameter_count(metadata.blob_at(row[4])))
        elif target_kind == FIELD:
            kind = "field"
            owner = type_name(field_owner[target])
            member = metadata.string(tables[FIELD][target - 1][1])
        elif target_kind == PROPERTY:
            kind = "property"
            owner = type_name(property_owner[target])
            member = metadata.string(tables[PROPERTY][target - 1][1])
        elif target_kind == EVENT:
            kind = "event"
            owner = type_name(event_owner[target])
            member = metadata.string(tables[EVENT][target - 1][1])
        else:
            continue
        rows.append({
            "version": version,
            "kind": kind,
            "owner": owner,
            "member": member,
            "parameters": count,
            "diagnostic": diagnostic,
            "message": (message or "").replace("\t", " ").replace("\n", " "),
        })
    return rows


def row_order(row: dict[str, str]) -> tuple[str, ...]:
    return (row["version"], row["owner"], row["member"], row["kind"], row["parameters"])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--package", required=True, type=Path, action="append")
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    rows = []
    for package in args.package:
        version = re.search(r"(\d+\.\d+\.\d+)", package.name).group(1)
        with zipfile.ZipFile(package) as archive:
            for entry in sorted(archive.namelist()):
                if entry.startswith("ref/") and entry.rsplit("/", 1)[-1] in ASSEMBLIES:
                    rows.extend(obsolete_rows(Metadata(archive.read(entry)), version))
    rows.sort(key=row_order)
    fields = ["version", "kind", "owner", "member", "parameters", "diagnostic", "message"]
    with args.output.open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, dialect="excel-tab", lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    print(f"{len(rows)} obsolete entries from {len(args.package)} packages")


if __name__ == "__main__":
    main()
