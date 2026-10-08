#!/usr/bin/env python3
"""Inspect a private reference file and emit reproducible metadata."""
import argparse, hashlib, json, pathlib, struct

def sha256(path):
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("path")
    ap.add_argument("--out", default="reference-report.json")
    args = ap.parse_args()
    p = pathlib.Path(args.path)
    if not p.is_file():
        raise SystemExit("not a file: " + str(p))
    head = p.read_bytes()[:64]
    report = {
        "file": p.name,
        "size": p.stat().st_size,
        "sha256": sha256(p),
        "header_hex": head.hex(),
        "signatures": [],
    }
    signatures = {
        b"\x7fELF": "ELF",
        b"PK\x03\x04": "ZIP",
        b"PBP": "PBP",
    }
    for sig, name in signatures.items():
        if head.startswith(sig):
            report["signatures"].append(name)
    if head[:4] == b"\x7fELF" and len(head) >= 20:
        report["elf_class"] = {1: 32, 2: 64}.get(head[4], "unknown")
        report["elf_endian"] = {1: "little", 2: "big"}.get(head[5], "unknown")
        if head[4] == 1 and head[5] == 1:
            report["elf_machine"] = struct.unpack_from("<H", head, 18)[0]
    pathlib.Path(args.out).write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))

if __name__ == "__main__":
    main()
