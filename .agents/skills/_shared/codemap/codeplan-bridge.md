# CodePlan To CodeMap Bridge

This contract defines how CodePlan steps interact with CodeMap. Read it only when a `cp-*` skill needs to recommend, run, or verify CodeMap synchronization from a CodePlan step.

Also read the sibling `core.md` file in this directory.

## Ownership

`cp-sync` owns CodeMap updates for CodePlan work. Other `cp-*` skills may recommend `cp-sync` or report CodeMap findings, but should not hand-edit CodeMap unless the user explicitly asks for that skill to do so.

`cp-sync` only updates existing module `index.md` files. If a target CodeMap document is missing, report it and stop. Do not create missing CodeMap directories or `index.md` files.

## Source of Truth

The `File Operations` section at the start of the target `stepX.md` is the sole source of truth for sync scope.

The list must contain these four groups:

- `Edit`
- `Add`
- `Delete`
- `Move`

Stop and report if the list:

- is missing
- lacks any of the four groups
- uses wildcards, directory globs, or phrases like `相关文件`, `若干文件`, or `该目录下所有文件`
- lists a move without the `old/path -> new/path` form
- does not give exact file paths

## Step Selection

For sync-only work, use the most recently completed step that has not yet been synced when the user's request implies that behavior.

If multiple steps could apply or the choice is ambiguous, ask the user before proceeding.

## Directory Mapping

From each file entry, infer affected source directories:

- `Add` / `Edit`: map to the directory containing the file
- `Delete`: map to the directory where the file used to live
- `Move`: map to both the old directory and the new directory

Only source directories inside the default coverage roots from `core.md` matter unless the user explicitly asks otherwise.

## Update Scope

For each affected source directory:

1. Compute the CodeMap path using `core.md` path mapping.
2. If the `index.md` exists, update its immediate file and immediate submodule entries.
3. If the directory is new and its parent already has a CodeMap, update the parent's `直属子模块` list.
4. If the `index.md` or parent `index.md` is missing, record the missing path and stop without creating it.

Use `git diff` only to verify that no affected file was missed by the step's `File Operations`. Do not use the diff to expand the sync scope beyond the list unless the user explicitly confirms.

## Verification Behavior

Report a CodeMap finding when:

- a navigation-worthy change added a feature/module, moved or relocated files/features/modules, split or merged modules, or deleted files/features/modules, but `cp-sync` was not run and an existing CodeMap stayed stale
- CodeMap was updated for an ordinary bug fix, local correctness fix, performance tweak, internal refactor without file/module movement, or implementation-detail-only change
- a relevant CodeMap `index.md` is missing and the change is navigation-worthy; phrase this as a recommendation to run `cm-full` backfill, not as a hard failure

Do not report CodeMap findings for ordinary bug fixes or implementation-only edits when module navigation remains accurate.
