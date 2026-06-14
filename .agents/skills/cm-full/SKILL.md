---
name: cm-full
description: Full CodeMap maintenance. Use to synchronize, refresh, check, backfill, or generate Doc/CodeMap documentation from a source directory, file list, diff, or CodePlan step. Use for broad or recursive CodeMap work; use cm-light for current-context edits only.
disable-model-invocation: true
---

# CodeMap Full

## Purpose

Maintain `Doc/CodeMap/` as a source-module navigation index.

Use `cm-full` for broad CodeMap work:

- refresh an existing source directory CodeMap
- backfill missing CodeMap documents
- generate a CodeMap subtree
- check CodeMap consistency for a source scope
- update CodeMap from an explicit source directory, file list, diff, or CodePlan step

Use `cm-light` instead when the task only asks whether the current conversation's edits should update existing CodeMap files.

Do not compile or build the project.

## Required Reads

Before acting, read:

1. `AGENTS.md`
2. `Doc/CodeMap/README.md`
3. the requested source files, directories, diff, or CodePlan step
4. existing mapped `Doc/CodeMap/.../index.md` files when they exist

If `Doc/CodeMap/README.md` is missing, report it and ask whether to create it before continuing.

## Scope

Default source coverage:

- `CubeEngine/Application`
- `CubeEngine/EngineSrc`

Do not scan `external/`, `build/`, `Data/`, `Res/`, `VulkanShaders/`, `Tool/`, or other roots unless the user explicitly asks.

## Source Inspection Gate

Before writing any module `index.md`, inspect the corresponding source directory and read the relevant source files. Do not generate CodeMap content from directory names, filenames, existing CodeMap text, or guesses alone.

For every affected source directory:

1. List immediate participating source files and immediate source subdirectories.
2. Read enough of each immediate participating source file to understand its responsibility.
3. For sibling `.h/.cpp` or similar pairs, inspect both sides unless one side is missing, empty, or mechanically obvious after reading the other side.
4. For each immediate source subdirectory, first read its mapped lower-level CodeMap `index.md` when present.
5. If a lower-level `index.md` is missing, stale-looking, or too vague, inspect the subdirectory names and representative files before describing the submodule.
6. Only then update the mapped `Doc/CodeMap/.../index.md`.

If the requested scope is too large to inspect honestly in one pass, stop and recommend splitting the refresh by source directory.

## Bottom-Up Rule

CodeMap is built bottom-up.

When lower-level `index.md` files exist, use them to summarize parent `直属子模块` bullets instead of repeatedly reading all descendant source files. This reuse applies only to submodule summaries. The parent directory's own immediate source files still require source inspection.

If the user requests generation for an intermediate directory and non-ignored recursive subdirectories lack mapped CodeMap `index.md` files:

1. Stop before writing.
2. Explain that missing child CodeMaps must be generated first.
3. List missing descendant CodeMap documents.
4. Ask for confirmation to proceed with post-order generation.

After confirmation, generate descendants first, then synthesize parent directories from child `index.md` files.

## Update Existing Only

When asked to sync or refresh CodeMap, update only existing `index.md` files.

If a mapped `index.md` is missing, report the missing path and stop unless the user explicitly confirmed backfill/generation for this turn.

## Backfill / Generate

Backfill and generation require two-step confirmation:

1. Analyze the requested source root or intermediate directory.
2. List every missing `CM_` directory and `index.md`, including descendants.
3. Stop and ask for explicit confirmation.
4. Only after confirmation, create the listed documents in post-order.

Make clear that confirmation will create subordinate module documents, not just the top-level file.

## Description Rules

Follow `Doc/CodeMap/README.md` for path mapping, document shape, file rules, ignored paths, and update policy.

Additional writing rules:

- Keep descriptions directory-level.
- Do not explain class internals, function bodies, call chains, or historical changes.
- For intermediate directories, do not mention direct class names, method names, or descendant file names in `概览` or `直属子模块`.
- Use stable domain terms when they are the natural concept name, such as `Material` in rendering.
- If responsibility is unclear after inspection, write a conservative description instead of guessing.

## Inputs

Accept any of these:

- source directory path
- source file list
- diff or patch
- CodePlan step `文件操作清单`

Infer affected source directories from the input and map them to `Doc/CodeMap/CM_.../index.md`.

## Output

End with:

- source directories inspected
- CodeMap files updated
- missing CodeMap files, if any
- whether backfill/generation was blocked pending confirmation
- whether any files were skipped or only partially inspected
- confirmation that no compilation was run
