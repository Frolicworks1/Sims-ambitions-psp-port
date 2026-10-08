#!/usr/bin/env python3
"""Prepare a private local reference-data pack from a legally obtained Sims 3 Ambitions IPA."""
from __future__ import annotations
import argparse, hashlib, json, struct, zipfile
from pathlib import Path

M3G_ID=b"\xABJSR184\x0D\x0A\x1A\x0A"

def sha256(p):
    h=hashlib.sha256()
    with p.open("rb") as f:
        for b in iter(lambda:f.read(1024*1024),b""): h.update(b)
    return h.hexdigest()

def inspect_m3g(data):
    if not data.startswith(M3G_ID): return None
    if len(data) < 30: return {"identifier":"M3G","truncated":True}
    return {"identifier":"M3G","compression":data[12],
            "section_length":struct.unpack("<I",data[13:17])[0],
            "uncompressed_length":struct.unpack("<I",data[17:21])[0],
            "object_type":data[21],
            "header_length":struct.unpack("<I",data[22:26])[0],
            "version":[data[26],data[27]]}

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("ipa",type=Path)
    ap.add_argument("--out",type=Path,default=Path("private/reference"))
    args=ap.parse_args()
    if not args.ipa.is_file(): raise SystemExit("IPA not found: "+str(args.ipa))
    args.out.mkdir(parents=True,exist_ok=True)
    manifest={"ipa":{"name":args.ipa.name,"size":args.ipa.stat().st_size,"sha256":sha256(args.ipa)},"files":[]}
    with zipfile.ZipFile(args.ipa) as z:
        names=z.namelist()
        app=next((n for n in names if n.endswith(".app/sims3dp_iphone")),None)
        if app:
            raw=z.read(app)
            (args.out/"sims3dp_iphone").write_bytes(raw)
            manifest["executable"]={"path":app,"size":len(raw),
                                    "sha256":hashlib.sha256(raw).hexdigest(),
                                    "mach_o_magic":raw[:4].hex()}
        for n in names:
            if n.endswith("/"): continue
            if n.lower().endswith(".m3g"):
                raw=z.read(n)
                item={"path":n,"size":len(raw),
                      "sha256":hashlib.sha256(raw).hexdigest()}
                h=inspect_m3g(raw)
                if h: item.update(h)
                manifest["files"].append(item)
                if Path(n).name in {"scene_town_map_dp.m3g","sim_male.m3g","sim_female.m3g","mini_sim_player.m3g"}:
                    (args.out/Path(n).name).write_bytes(raw)
    manifest["m3g_count"]=sum(1 for x in manifest["files"] if x.get("identifier")=="M3G")
    (args.out/"reference-manifest.json").write_text(json.dumps(manifest,indent=2),encoding="utf-8")
    print(json.dumps({"output":str(args.out),"m3g_count":manifest["m3g_count"],
                      "executable":manifest.get("executable")},indent=2))

if __name__=="__main__":
    main()
