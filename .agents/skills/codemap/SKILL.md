---
name: codemap
description: Synchronize, refresh, update, check, backfill, or generate CodeMap documentation under Doc/CodeMap/ from a source directory, file list, diff, or CodePlan step. Default scope is CubeEngine/Application and CubeEngine/EngineSrc.
---

# CodeMap Maintenance

## Purpose

Keep `Doc/CodeMap/` synchronized with selected source directories so humans and AI can locate modules quickly without re-reading the whole codebase.

CodeMap documents do **not** explain implementation, call chains, or class internals. They only describe:

- which module a source directory represents
- what files live directly in that directory
- what subdirectories live directly under it

## Default Coverage

First version covers only:

- `CubeEngine/Application`
- `CubeEngine/EngineSrc`

Do not scan `external/`, `build/`, `Data/`, `Res/`, `VulkanShaders/`, `Tool/`, or other roots unless the user explicitly asks.

## Path Mapping Rule

A source directory is mirrored under `Doc/CodeMap/` with a `CM_` prefix on every directory level. The module document is always named `index.md`.

Example:

```text
Source:     CubeEngine/EngineSrc/Rendering
CodeMap:    Doc/CodeMap/CM_CubeEngine/CM_EngineSrc/CM_Rendering/index.md
```

The `CM_` prefix prevents fuzzy-search collisions between real source directories and CodeMap mirrors in editors.

## Document Shapes

Each `index.md` describes only the corresponding source directory's immediate contents. Do not recursively list descendant files.

### Leaf code node

The directory directly contains participating source files and has no source subdirectories.

Content: title, overview, immediate files.
Omit the "Immediate Submodules" section.

### Grouping node

The directory has no immediate source files but has source subdirectories.

Content: title, overview, immediate submodules.
Omit the "Immediate Files" section.

### Mixed node

The directory directly contains source files and also has source subdirectories.

Content: title, overview, immediate files, immediate submodules.

### Template

```md
# CubeEngine/EngineSrc/Rendering

## 概览

One sentence describing this directory's responsibility.

## 直属文件

- `Renderer.h/.cpp`: one-line description of what this file pair does.

## 直属子模块

- `vk/`: Vulkan backend implementation.
- `gl/`: OpenGL backend implementation.
```

Omit any section that has no entries.

## File Whitelist

Participating source file types:

- C/C++ sources: `.c`, `.cc`, `.cpp`, `.cxx`
- C/C++ headers: `.h`, `.hh`, `.hpp`, `.hxx`
- Inline implementation: `.inl`, `.ipp`

By default, merge sibling `.h/.cpp` pairs into a single bullet unless they clearly describe different concepts.

## Ignored Files

Do not list or mention by default:

- Backup / temp / old files: `.old`, `.bak`, `.tmp`, `.orig`
- Build scripts and project files: `CMakeLists.txt`, `.cmake`, `.sln`, `.vcxproj`
- Compiled artifacts: `.obj`, `.o`, `.lib`, `.dll`, `.exe`, `.pdb`, `.ilk`
- Editor / IDE local files: `.suo`, `.user`, `.vcxproj.user`
- Assets and data files: images, audio, models, archives, JSON data tables
- Third-party and build directories: `external/`, `build/`

If the user explicitly asks for build-system CodeMap, add that scope in a separate request.

## Default Behavior: Update Only

When asked to sync or refresh CodeMap, only update existing `index.md` files under `Doc/CodeMap/`.

If the target CodeMap directory or `index.md` is missing, stop and report the missing paths. Do not create anything until the user gives explicit confirmation in a follow-up turn.

## Backfill Behavior: Two-Step Confirmation

If the user asks to backfill or generate missing CodeMap documents:

1. Analyze the requested source root or intermediate directory.
2. List every missing `CM_` directory and `index.md` by level, including the start directory and all missing descendants.
3. Stop and wait for the user's explicit confirmation.
4. Only after confirmation, recursively create the listed directories and `index.md` files.

The report must make clear that confirming will create not just the top-level directory but also all missing subordinate module documents.

## README.md

`Doc/CodeMap/README.md` records global conventions: goals, coverage, path mapping, document shapes, file whitelist, and ignore rules. Treat it as part of the CodeMap system, not as a module document tied to a source directory.

If `README.md` is missing, include it in the missing-items report and wait for confirmation before creating it.

## Deletions and Moves

- When a source file is deleted, remove its entry from the corresponding module's `index.md`.
- When a file moves, treat it as a deletion from the old module and an addition to the new module.
- If a move changes which directory a file belongs to, update both the old and new module `index.md`.

## Inputs

Accept any of these as the starting point:

- a source directory path
- a list of source files
- a diff or patch
- a CodePlan step's `文件操作清单`

From the input, infer the affected source directories and update only their existing mapped `index.md` files.

## Non-Goals

- Do not explain class internals, functions, or call chains.
- Do not keep historical change notes in module documents.
- Do not add philosophical architecture essays.
- Do not compile or build the project.
- Do not create, move, or delete CodeMap files unless the user has explicitly confirmed it in the current or previous turn.
