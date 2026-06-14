---
name: cm-light
description: Lightweight current-context CodeMap synchronization. Use after ordinary non-CodePlan edits when Codex should inspect only files changed in the current conversation or current git diff, ask the user whether CodeMap should be updated, and then update existing Doc/CodeMap index.md files only for navigation-worthy changes.
disable-model-invocation: true
---

# CodeMap Light

## Shared Contract

Before acting, read `../_shared/codemap/core.md`.

## Purpose

Synchronize existing CodeMap documents from the current conversation's file changes. This is the lightweight companion to `cm-full`.

Use this skill when work was done without a CodePlan step and the user wants to consider whether the current edits should update `Doc/CodeMap/`.

Do not refresh directories broadly, backfill missing CodeMaps, or generate a CodeMap tree.

## Required Reads

Before deciding anything, read:

1. `AGENTS.md`
2. `../_shared/codemap/core.md`
3. the current changed-file list from conversation context and, when useful, `git diff --name-only` or `git status --short`
4. affected source files that are candidates for CodeMap updates
5. corresponding existing CodeMap `index.md` files, if present

## Confirmation First

Always ask the user before editing CodeMap.

First reply with:

- candidate source files changed in the current context
- whether the change appears navigation-worthy
- mapped existing CodeMap files, if any
- missing CodeMap files, if any
- a yes/no question asking whether to update the existing CodeMap files

Do not edit CodeMap until the user confirms.

## Scope

Use only current-context changes:

- files edited, added, deleted, or moved during the current conversation
- files visible in the current git diff or status when that helps confirm the set

If unrelated dirty files exist, do not include them unless the user explicitly includes them.

If the changed-file set is unclear, ask the user to confirm the file list.

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

If the change is not navigation-worthy, recommend skipping CodeMap and stop.

## Update Rules

- Update existing `Doc/CodeMap/.../index.md` files only.
- Do not create missing CodeMap directories or `index.md` files; recommend `cm-full` backfill instead.
- Follow the shared CodeMap contract's mapping and document-shape rules.
- Read affected source files before changing file bullets.
- Reuse child `index.md` files for parent submodule summaries when available.
- Keep descriptions directory-level and avoid implementation details.
- Do not compile or build the project.

## Output

End with:

- whether CodeMap was updated, skipped, or blocked
- changed source files considered
- CodeMap files updated
- missing CodeMap files, if any
- confirmation that no compilation was run
