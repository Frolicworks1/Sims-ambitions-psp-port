# PSP Port Status

## Reference acquired

The iOS IPA is a real native reference for The Sims 3: Ambitions 1.1.84. It contains a universal Mach-O executable (sims3dp_iphone) with ARMv7, ARMv7s and ARM64 slices and a large M3G resource set.

## What this proves

The target is a native engine using OpenGLES/OpenAL/CoreAudio and Java-ME M3G-compatible resources. The PSP target therefore needs a MIPS-native runtime plus an asset and engine compatibility layer.

## Current PSP implementation

The repository builds a valid PSP ELF/EBOOT bootstrap. It is not yet the game.

## Engine milestones

1. Decode and validate M3G resources used by the game.
2. Implement a PSP-native M3G scene/resource loader.
3. Replace OpenGLES calls with a PSP GU backend.
4. Replace OpenAL/CoreAudio with PSP audio.
5. Map touch/UI/input semantics to PSP controls.
6. Port the game loop/state machine identified from the ARM reference.
7. Add a private asset pack and produce a local playable EBOOT.

The IPA and extracted proprietary assets stay outside the public repository.
