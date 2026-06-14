---
name: cp-sync
description: Synchronize CodeMap documentation from a CodePlan step, a completed step, the next step, a specified stepX.md, or a file operation list. Reads the step's 文件操作清单 and updates existing CodeMap index.md files only.
---

# CodePlan CodeMap Sync

## Purpose

Bridge a CodePlan step to the existing CodeMap documents under `Doc/CodeMap/`.

`cp-sync` does **not** apply code changes. It only updates existing module `index.md` files that correspond to source directories affected by the step's `文件操作清单`.

If a target CodeMap document is missing, report it and stop. Do not create it.

## Required Reads

Before doing anything, read:

1. `AGENTS.md`
2. `CodePlan/<PlanName>/design.md`
3. `CodePlan/<PlanName>/progress.md`
4. The target `stepX.md`
5. The affected source directories
6. The corresponding existing CodeMap `index.md` files

## Plan Selection

Use the same rules as `cp-apply` and `cp-verify`:

- If the user names a plan directory under `CodePlan/`, use it.
- If the user says `latest` or `最新`, pick the plan whose `design.md`, `progress.md`, or `step*.md` is most recently modified.
- If only one plan exists, use it unless the user says otherwise.
- If multiple plans exist and the target is ambiguous, ask the user to choose. Always include `latest/最新` as an option.

## Step Selection

- If the user names `stepN`, use that step.
- Otherwise look at `progress.md` and choose the most recently completed step that has not yet been synced.
- If multiple steps could apply or the choice is ambiguous, ask the user before proceeding.

## Source of Truth

The `文件操作清单` at the start of the target `stepX.md` is the sole source of truth for the sync scope.

The list must contain these four groups:

- 编辑
- 新增
- 删除
- 移动

If any group is missing, or the list is absent, or paths are vague, stop and report that the step must be fixed first.

## Invalid Lists

Stop and report if the `文件操作清单`:

- is missing
- lacks any of the four groups (`编辑`, `新增`, `删除`, `移动`)
- uses wildcards, directory globs, or phrases like "相关文件" / "若干文件" / "该目录下所有文件"
- lists a move without the `old/path -> new/path` form
- does not give exact file paths

## Directory Mapping

From each file entry, infer the affected source directory:

- **新增 / 编辑**: map to the directory containing the file.
- **删除**: map to the directory where the file used to live.
- **移动**: map to both the old directory and the new directory.

Only source directories inside the default coverage roots (`CubeEngine/Application`, `CubeEngine/EngineSrc`) matter unless the user explicitly asks otherwise.

## Update Scope

For each affected source directory:

1. Compute the CodeMap path: `Doc/CodeMap/CM_<dir>/.../index.md`.
2. If the `index.md` exists, update its immediate file and immediate submodule entries.
3. If the directory is new and its parent already has a CodeMap, update the parent's `直属子模块` list.
4. If the `index.md` or parent `index.md` is missing, record the missing path and stop without creating it.

## Missing CodeMap Behavior

If any target `index.md` does not exist, list every missing path and stop. Do not create missing CodeMap directories or files. The user must confirm creation in a separate turn (handled by the `codemap` skill).

## Git Diff Check

You may use `git diff` only to verify that no affected file was missed by the step's `文件操作清单`. Do not use the diff to expand the sync scope beyond the list unless the user explicitly confirms.

## Output

End with a concise report:

- Source plan and step
- Source directories mapped from the `文件操作清单`
- CodeMap files updated
- CodeMap files missing (not created)
- Any issues with the `文件操作清单`
- Confirmation that no compilation was run

## Non-Goals

- Do not apply code changes.
- Do not edit `progress.md` unless the user explicitly asks to record sync state.
- Do not create new `Doc/CodeMap/` directories or `index.md` files.
- Do not scan the whole source tree.
- Do not include build files (`CMakeLists.txt`, `.cmake`, `.sln`, `.vcxproj`) in the default sync scope.
- Do not compile or build the project.
