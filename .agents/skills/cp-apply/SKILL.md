---
name: cp-apply
description: Apply exactly one CodePlan step to the repository, with preflight idempotency checks and progress.md updates. Use when the user asks to execute, apply, continue, or implement a specific step or the next unchecked step.
disable-model-invocation: true
---

# CodePlan Apply

## Shared Contract

Before acting, read:

1. `../_shared/codeplan/core.md`
2. `../_shared/codeplan/step-contract.md`

## Purpose

Implement exactly one `stepX.md` at a time. Run preflight checks first so repeated or partially-applied steps are detected before editing.

## Required Reads

Before editing, read:

1. `AGENTS.md`
2. `CodePlan/<PlanName>/design.md`
3. `CodePlan/<PlanName>/progress.md`
4. the requested or inferred `stepX.md`

Read other step files only when the current step explicitly depends on them.

## Preflight Idempotency

Use the shared `step-contract.md` preflight state rules before editing.

## Apply Rules

- Implement only the current step.
- Do not do future-step work.
- Do not perform opportunistic refactors.
- Do not compile or build this repository unless the user explicitly asks.
- Do not revert user changes.
- Do not run destructive git commands.

## Progress Updates

Use the shared `step-contract.md` progress update shape.

## Plan Completion

After marking the current step complete, check whether it is the last unchecked step listed in `progress.md` (i.e., all steps are now checked).

If this was the final step:

1. State explicitly in the final response that the entire CodePlan is now complete (`CodePlan/<PlanName> 的所有步骤已完成`).
2. Based only on the `File Operations` entries in the plan's `stepX.md` files, tell the user whether the completed plan may need a later `cp-sync` pass.
3. Treat `cp-sync` as only possibly needed when the step file changes suggest navigation-level changes: new source files or modules, deleted source files or modules, moved files, feature relocation, module split, or module merge.
4. Do not read existing CodeMap files and do not judge whether CodeMap is stale from `cp-apply`.
5. Phrase the `cp-sync` note explicitly as a possibility, not a decision, for example: `Based on the step File Operations, this CodePlan may need a later cp-sync pass to update CodeMap.`
6. Ask the user whether they want to run verification for the whole plan (`是否需要对当前 CodePlan 进行校验？`).
7. Do not start verification or `cp-sync` on your own; wait for the user's answer.

If this was not the final step, continue normally and do not ask for whole-plan verification.

## Verification

Prefer non-build verification:

- inspect changed code paths
- search for stale references with `rg`
- check CMake target names and paths textually
- review headers and include relationships
- run lightweight static commands only if they do not build

If compilation would be the only strong verification, say it was intentionally not run.

## Final Response

Summarize:

- selected plan and step
- preflight classification
- files changed
- progress update
- final-plan `cp-sync` possibility note, only when this step completed the whole progress
- verification performed or skipped
