# Private reference workspace

Place legally obtained reference binaries/packages here locally only. Do not commit proprietary game files.

Workflow:
1. Keep the original reference outside the repository when possible.
2. Run `tools/inspect_reference.py` against it.
3. Save only non-sensitive metadata if appropriate.
4. Record confirmed architecture/ABI/file-format findings in `docs/REVERSE_ENGINEERING.md`.
