# The Sims 3: Ambitions — PSP Porting Plan

## Goal
Develop a lawful, research-driven PSP implementation/porting project targeting PPSSPP and real PSP hardware where technically feasible.

## Scope
- Analyze user-provided legally obtained game binaries and documentation.
- Identify executable architecture, engine/runtime boundaries, file formats, rendering, audio, input, filesystem and memory systems.
- Build PSP-native replacements/adapters where required.
- Optimize for PSP CPU, RAM and GU constraints.
- Produce a reproducible PSP build pipeline and EBOOT.PBP from project-owned source.

## Milestones
1. Repository/toolchain bootstrap
2. Binary and format inventory
3. Static/reverse-engineering notes
4. Runtime/engine boundary mapping
5. PSP platform abstraction
6. Renderer/GU prototype
7. Input/audio/filesystem adapters
8. Asset loading and memory/streaming strategy
9. Engine integration
10. Build, boot and runtime validation
11. Performance/stability optimization

## Acceptance criteria
A build is not considered playable until it boots in PPSSPP, reaches the game runtime, renders a usable scene, accepts player input, and remains stable during basic gameplay.

## Legal boundary
Do not commit copyrighted game ISOs, executables, extracted game assets, or other redistributable proprietary content to this repository. Keep private game data outside Git and document only the technical interfaces/metadata needed for the project.
