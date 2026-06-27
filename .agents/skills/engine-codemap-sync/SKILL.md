---
name: engine-codemap-sync
description: Assess whether completed engine code changes should be reflected in Doc/EngineCodeMap. Use implicitly only after C/C++ source or header file changes under CubeEngine/EngineSrc, such as .c, .cc, .cpp, .cxx, .h, .hh, .hpp, .hxx, .inl, or .ipp, and only when the change may alter durable module responsibilities, important objects, or stable entry points that help narrow source search. Ignore doc-only, build-only, asset-only, config-only, generated, formatting, and other non-code-file changes unless the user explicitly asks to evaluate CodeMap. Also use when the user says "更新codemap", "update codemap", or otherwise explicitly asks to update or sync CodeMap. First filter out pure bug fixes, tiny helper-only changes, formatting, generated files, assets, build-only changes, and implementation-only refactors that do not change AI navigation value. If classified as skip, do not ask the user; only ask before editing when a real CodeMap update is likely needed.
---

# Engine CodeMap Sync

## Workflow

1. Inspect changed files from context or `git diff --name-only`.
2. Treat only C/C++ source or header files as trigger files: `.c`, `.cc`, `.cpp`, `.cxx`, `.h`, `.hh`, `.hpp`, `.hxx`, `.inl`, and `.ipp`.
3. For implicit post-change use, if there are no changed trigger files under `CubeEngine/EngineSrc/`, report `skip`. Ignore docs, build files, assets, generated files, scripts, config, shaders, data files, and other non-code-file changes when deciding whether the skill should run implicitly.
4. If the user explicitly asks to evaluate or update CodeMap, the skill may still run without trigger files, but treat non-code changes as supporting context and classify `skip` unless they clearly affect durable source navigation.
5. Map each changed engine trigger file by first-level directory: `CubeEngine/EngineSrc/<Module>/...` -> `Doc/EngineCodeMap/<Module>.md`.
6. Read `Doc/EngineCodeMap/README.md`, the mapped CodeMap files, and only the changed source needed to judge navigation impact.
7. Classify as `skip`, `needs-update`, or `blocked`.
8. For `skip`, stop without asking; at most mention the reason in the final summary.
9. For `needs-update`, summarize the proposed durable facts and ask for confirmation before editing.
10. Include `Doc/EngineCodeMap/README.md` in the proposed edit set when a first-level engine directory is added, removed, renamed, or materially repurposed.
11. After confirmation, patch only the smallest CodeMap file set and run `git diff --check` on edited docs.

## Update Bar

Update only for durable AI-navigation changes: module responsibilities, important objects, and stable entry points that help decide which source file or class to open.

Skip pure bug fixes, tiny private helpers, formatting, comments-only cleanup, generated files, assets, build-only changes, and implementation-only refactors with no navigation impact.

## Style

- One CodeMap file per first-level `EngineSrc` directory.
- `Doc/EngineCodeMap/README.md` is the directory index; keep its module list aligned with first-level `CubeEngine/EngineSrc` directories.
- Fold second-level directories into the parent module file, for example `3D/Terrain` belongs in `3D.md`, and `BackEnd/vk` belongs in `BackEnd.md`.
- Start each module with `## Role`: one short paragraph that says what this module helps locate.
- Use `## Important Objects` for stable navigation anchors: classes, files, or systems that are good first places to open.
- Each object entry should explain the search reason in one sentence.

## Output

Report classification, reason, engine files considered, mapped CodeMap files, proposed durable facts if any, confirmation status, updated files if any, and verification.
