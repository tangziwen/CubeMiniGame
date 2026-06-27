---
name: engine-codemap-sync
description: Assess whether completed engine code changes should be reflected in Doc/EngineCodeMap. Use implicitly after code changes when changed files include CubeEngine/EngineSrc and the change may alter important engine objects, module responsibilities, entry points, ownership contracts, boundaries, or known traps. Also use when the user says "更新codemap", "update codemap", or otherwise explicitly asks to update or sync CodeMap. First filter out pure bug fixes, tiny helper-only changes, formatting, generated files, assets, and implementation-only refactors that do not change AI navigation value. If classified as skip, do not ask the user; only ask before editing when a real CodeMap update is likely needed.
---

# Engine CodeMap Sync

## Workflow

1. Inspect changed files from context or `git diff --name-only`.
2. If no file is under `CubeEngine/EngineSrc/`, report `skip`.
3. Map each changed engine file by first-level directory: `CubeEngine/EngineSrc/<Module>/...` -> `Doc/EngineCodeMap/<Module>.md`.
4. Read `Doc/EngineCodeMap/README.md`, the mapped CodeMap files, and only the changed source needed to judge navigation impact.
5. Classify as `skip`, `needs-update`, or `blocked`.
6. For `skip`, stop without asking; at most mention the reason in the final summary.
7. For `needs-update`, summarize the proposed durable facts and ask for confirmation before editing.
8. After confirmation, patch only the smallest mapped CodeMap file set and run `git diff --check` on edited docs.

## Update Bar

Update only for durable AI-navigation changes: important objects, module responsibilities, entry points, runtime/dispatch order, ownership/lifetime contracts, module boundaries, or known traps.

Skip pure bug fixes, tiny private helpers, formatting, comments-only cleanup, generated files, assets, build-only changes, and implementation-only refactors with no navigation impact.

## Style

- One CodeMap file per first-level `EngineSrc` directory.
- Never create second-level CodeMap files; fold `3D/Terrain` into `3D.md`, `BackEnd/vk` into `BackEnd.md`, etc.
- Keep text short, object-centered, and AI-facing.
- Do not write file inventories, helper internals, routine bug-fix notes, validation details, or tutorial prose.

## Output

Report classification, reason, engine files considered, mapped CodeMap files, proposed durable facts if any, confirmation status, updated files if any, and verification.
