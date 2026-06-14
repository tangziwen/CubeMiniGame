---
name: cp-sync
description: Synchronize CodeMap documentation from a CodePlan step, a completed step, the next step, a specified stepX.md, or a File Operations list. Reads the step's File Operations and updates existing CodeMap index.md files only.
disable-model-invocation: true
---

# CodePlan CodeMap Sync

## Shared Contract

Before acting, read:

1. `../_shared/codeplan/core.md`
2. `../_shared/codeplan/step-contract.md`
3. `../_shared/codemap/core.md`
4. `../_shared/codemap/codeplan-bridge.md`

## Purpose

Bridge a CodePlan step to the existing CodeMap documents under `Doc/CodeMap/`.

`cp-sync` does **not** apply code changes. It only updates existing module `index.md` files that correspond to source directories affected by the step's `File Operations`.

If a target CodeMap document is missing, report it and stop. Do not create it.

`cp-sync` must also decide whether the completed step is worth writing to CodeMap at all. Existing CodeMap files should stay unchanged for implementation-only work.

## Required Reads

Before doing anything, read:

1. `AGENTS.md`
2. `CodePlan/<PlanName>/design.md`
3. `CodePlan/<PlanName>/progress.md`
4. The target `stepX.md`
5. The affected source directories
6. The corresponding existing CodeMap `index.md` files

## Source of Truth

Use the shared CodeMap contracts for source-of-truth, invalid-list, directory-mapping, sync-worthiness, update-scope, and missing-CodeMap behavior.

## Git Diff Check

Use `git diff` only as allowed by the shared CodeMap contracts.

## Output

End with a concise report:

- Source plan and step
- Sync worthiness classification and reason
- Source directories mapped from `File Operations`
- CodeMap files updated
- CodeMap update skipped, if the step was not navigation-worthy
- CodeMap files missing (not created)
- Any issues with `File Operations`
- Confirmation that no compilation was run

## Non-Goals

- Do not apply code changes.
- Do not edit `progress.md` unless the user explicitly asks to record sync state.
- Do not create new `Doc/CodeMap/` directories or `index.md` files.
- Do not scan the whole source tree.
- Do not include build files (`CMakeLists.txt`, `.cmake`, `.sln`, `.vcxproj`) in the default sync scope.
- Do not compile or build the project.
