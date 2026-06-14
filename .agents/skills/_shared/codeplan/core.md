# CodePlan Core Contract

This contract is shared by the `cp-*` skill family.

## Concept

A CodePlan is a scoped work package under `CodePlan/<PlanName>/`. The directory name should be compact PascalCase.

The standard files are:

- `design.md`: durable requirements, constraints, current state, target architecture, coarse phases, risks, and validation strategy
- `progress.md`: the executable checklist, completed summaries, and blockers or notes
- `stepX.md`: one precise implementation step per file

Design-level intent belongs in `design.md`. File-level execution details belong in `stepX.md`. Step completion and blockers belong in `progress.md`.

Protocol headings in `design.md`, `stepX.md`, and `progress.md` must be English and must match the shared contracts exactly. Chinese prose is allowed inside sections, but Chinese headings must not be used as protocol anchors.

Write each CodePlan so an implementation agent normally needs only:

- `design.md`
- `progress.md`
- the current `stepX.md`

## Phase Boundaries

Use the smallest CodePlan phase that matches the user's request:

- `cp-explore`: read-only requirement discovery before writing `design.md`
- `cp-plan`: create or update `design.md`
- `cp-refine`: turn `design.md` into `stepX.md` files and `progress.md`
- `cp-apply`: apply exactly one `stepX.md`
- `cp-review`: review CodePlan documents, not implementation code
- `cp-verify`: review implementation code against a specific step

Do not cross phase boundaries unless the user explicitly changes the task.

## Plan Selection

When `CodePlan/` contains multiple plan directories, resolve the target before reading or editing:

- If the user provides a directory name that exactly matches `CodePlan/<Name>`, use it directly.
- If the user says `latest` or `最新`, select the plan whose `design.md`, `progress.md`, or `step*.md` has the newest modification time.
- If there is only one plan directory, use it unless the user's request conflicts with it.
- If the request clearly creates a new plan and no exact existing name was given, choose a new compact PascalCase name.
- If multiple plans exist and the target is ambiguous, ask the user to choose. Always include `latest/最新` as one option and explain which plan it currently resolves to.

When the user asks to plan `latest` or `最新`, treat it as updating an existing plan and ask what area should be refined unless the requested edit is already specific.

## Idempotency

Never silently overwrite existing CodePlan documents.

Before updating an existing file:

1. Read the current file.
2. Summarize the existing intent or status when it matters.
3. Preserve stable decisions, completed summaries, and manual notes unless the user explicitly replaces them.
4. Warn when changing an upstream document may make downstream documents stale.

## Scope

A CodePlan should describe a work package roughly equivalent to one programmer's focused 3-day to 1-week task.

Use expected file count as the primary estimate:

- `small`: 1-10 files
- `medium`: 11-30 files
- `large`: 31-50 files
- `too large`: more than 50 files

Adjust the estimate with change shape:

- `small change`: local functions, a few fields, configuration, or one behavior point
- `medium change`: several modules, new interfaces, or non-trivial call-chain adjustments
- `large change`: architecture boundaries, object lifetimes, resource management, render pipeline behavior, or core data structures
- `too large change`: rewriting a subsystem, replacing a core technology stack, migrating language or graphics API, or broad add/delete/move operations

Line count is only an auxiliary signal. If it cannot be estimated reliably, do not invent an exact number; use the file-count estimate plus the fuzzy change-size category instead.

Do not create or update `design.md` as a single plan if any of these are likely true:

- more than 50 files will be added, edited, deleted, or moved
- more than 10 refined implementation steps would be needed
- the work is clearly a `too large change`
- the work is likely to exceed roughly 1000 changed lines, when that can be reasonably inferred

When the scope gate fails, propose a split by stable architecture boundaries such as investigation, interface isolation, one subsystem migration, compatibility cleanup, and final verification.

## Project Conventions

For this repository:

- Read `AGENTS.md` before touching repository behavior.
- Use UTF-8 for Markdown files.
- Preserve Chinese wording when it is clearer.
- Treat this as a single-person C++ engine project.
- Vulkan is the default renderer.
- Do not compile or build unless the user explicitly asks.
- Do not revert user changes.
- Do not run destructive git commands.
