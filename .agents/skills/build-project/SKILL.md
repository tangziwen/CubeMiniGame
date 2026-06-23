---
name: build-project
description: Build the project using the CMake presets defined for this repository. Use when the user asks to compile, build, or verify that the project builds.
---

# Build Project

## Shared Contract

Before acting, read:

1. `../_shared/codeplan/core.md`

## Purpose

Provide the single source of truth for building this project. All compile passes from other skills or workflows should use the commands defined here.

## Environment

The project's usual production environment is Windows with a Vulkan C++ engine.

Do not run commands that `cmd` or PowerShell cannot understand. Since build commands are typically invoked through PowerShell-style presets, use the exact commands below unless the user explicitly asks for another toolchain.

## Default Build Commands

Use the CMake Visual Studio preset by default:

```powershell
cmake --preset windows-msvc
cmake --build --preset windows-msvc-relwithdebinfo
```

Run them in the project root directory.

## Error Handling

If the build fails:

1. Capture the first few relevant compilation errors with file and line references.
2. Report them clearly.
3. Do not silently retry a clean build unless the user asks.
4. If another skill invoked this build as part of its workflow, return the result so the caller can decide whether to fix, record a blocker, or ask the user.

## Out of Scope

- This skill builds the project; it does not review code, apply CodePlan steps, or fix source code unless combined with another task.
- Do not modify `build/`, third-party directories, generated artifacts, or large asset packages as part of a normal build.
