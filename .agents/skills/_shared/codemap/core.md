# CodeMap Core Contract

This contract is the canonical maintenance rule set for skills that maintain `Doc/CodeMap/`.

## Required Reads

Before editing CodeMap, read:

1. `AGENTS.md`
2. affected source files or existing mapped CodeMap `index.md` files as required by the active skill

`Doc/CodeMap/README.md` is a short human-facing entry point, not the rule authority.

## Concept

`Doc/CodeMap/` is a navigation index for source modules. Source code remains the final authority.

Treat each participating source directory as a module boundary. Each module has one `index.md` that documents only that directory's immediate source files and immediate source subdirectories.

Build and refresh CodeMap bottom-up: leaf directories first, then parent directories synthesized from child `index.md` files.

Parent module summaries should describe submodule responsibilities, not descendant classes, methods, or file names.

## Scope

Default source coverage:

- `CubeEngine/Application`
- `CubeEngine/EngineSrc`

Do not scan or map ignored roots unless the user explicitly asks.

Do not include build files such as `CMakeLists.txt`, `.cmake`, `.sln`, or `.vcxproj` in default CodeMap scope.

Ignored roots by default:

- `external/`
- `build/`
- `Data/`
- `Res/`
- `VulkanShaders/`
- `Tool/`
- other third-party, generated, asset, or build output roots

## Path Mapping

Mirror each source directory under `Doc/CodeMap/` and add `CM_` to every directory level. The module document is always named `index.md`.

```text
Source:  CubeEngine/EngineSrc/Rendering
CodeMap: Doc/CodeMap/CM_CubeEngine/CM_EngineSrc/CM_Rendering/index.md

Source:  CubeEngine/Application/CubeGame
CodeMap: Doc/CodeMap/CM_CubeEngine/CM_Application/CM_CubeGame/index.md

Source:  CubeEngine/EngineSrc/Core/Math
CodeMap: Doc/CodeMap/CM_CubeEngine/CM_EngineSrc/CM_Core/CM_Math/index.md
```

The `CM_` prefix prevents fuzzy-search collisions between real source directories and CodeMap mirrors.

## Document Shape

Use only these sections when they have content:

```md
# CubeEngine/EngineSrc/Rendering

## 概览

One sentence describing this directory's module responsibility.

## 直属文件

- `Renderer.h/.cpp`: one-line description of the immediate file or file pair.

## 直属子模块

- `vk/`: one-line description of the immediate submodule responsibility.
```

Leaf modules usually have `概览` and `直属文件`.

Grouping modules usually have `概览` and `直属子模块`.

Mixed modules may have all three sections.

## Source Inspection

Before writing any module `index.md`, inspect the corresponding source directory and read the relevant immediate source files.

Do not generate CodeMap content from directory names, filenames, existing CodeMap text, or guesses alone.

For parent summaries, reuse child `index.md` files when they exist. This reuse applies only to submodule summaries; the parent directory's own immediate source files still require source inspection.

## Update Existing Only

When asked to sync or refresh CodeMap, update only existing `index.md` files unless the active skill and user confirmation explicitly allow backfill or generation.

If a mapped `index.md` is missing, report the missing path and stop unless the user explicitly confirmed backfill/generation for this turn.

If child CodeMaps are missing under an intermediate directory, generate children first, then the parent, but only when the active skill and user confirmation allow generation.

## Worthiness Gate

Update existing CodeMap only for navigation-worthy changes:

- new feature or new module
- file move, feature relocation, module split, or module merge
- file deletion, module deletion, or feature deletion

Do not update CodeMap for:

- ordinary bug fixes or crash fixes
- local correctness fixes
- performance optimization
- internal refactors that do not move files or change module boundaries
- implementation-detail changes inside already-documented files
- formatting, comments, logging, or diagnostics-only changes

If a change is not navigation-worthy, skip CodeMap updates and report the reason.

## Writing Rules

Keep descriptions directory-level.

Do not explain class internals, function bodies, call chains, or historical changes.

For intermediate directories, do not mention direct class names, method names, or descendant file names in `概览` or `直属子模块`.

Use stable domain terms when they are the natural concept name.

If responsibility is unclear after inspection, write a conservative description instead of guessing.

Do not compile or build the project.

## File Rules

Participating source files:

- `.c`, `.cc`, `.cpp`, `.cxx`
- `.h`, `.hh`, `.hpp`, `.hxx`
- `.inl`, `.ipp`

Ignored by default:

- backup, temp, or old files such as `.old`, `.bak`, `.tmp`, `.orig`
- build scripts and project files such as `CMakeLists.txt`, `.cmake`, `.sln`, `.vcxproj`
- compiled artifacts
- editor or IDE local files
- assets and data files
- third-party and build directories
