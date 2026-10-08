#!/usr/bin/env python3
from pathlib import Path
import json,sys,hashlib
root=Path(sys.argv[1] if len(sys.argv)>1 else "private/reference")
m=root/"reference-manifest.json"
if not m.is_file(): raise SystemExit("Missing reference-manifest.json; run build_reference_pack.py first.")
d=json.loads(m.read_text())
print("Reference:",d["ipa"]["name"])
print("IPA SHA256:",d["ipa"]["sha256"])
print("M3G files indexed:",d.get("m3g_count",0))
for k in ("scene_town_map_dp.m3g","sim_male.m3g","sim_female.m3g","mini_sim_player.m3g"):
    p=root/k
    if p.exists(): print(k,p.stat().st_size,hashlib.sha256(p.read_bytes()).hexdigest())
print("OK")
