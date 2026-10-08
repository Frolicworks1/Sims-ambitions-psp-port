# Reverse Engineering Notes

This document records technical findings needed to reproduce a PSP port without distributing proprietary game content.

## Initial checklist
- [ ] Identify executable/container format
- [ ] Determine CPU/ABI and compiler fingerprints
- [ ] Inventory sections and imports
- [ ] Identify strings and subsystem names
- [ ] Map initialization and main-loop candidates
- [ ] Map renderer entry points
- [ ] Map audio subsystem
- [ ] Map input/controller subsystem
- [ ] Map filesystem/archive access
- [ ] Identify memory allocators and streaming
- [ ] Record confirmed findings with evidence

## Evidence policy
Separate confirmed observations from hypotheses. Record offsets, function signatures, and tool versions when available. Do not commit proprietary binaries or extracted assets.
