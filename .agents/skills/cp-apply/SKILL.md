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

Implement exactly one `stepX.md` at a time. Run preflight checks first to detect repeated or partially-applied work before editing.

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

After marking the current step complete, check whether all steps in `progress.md` are now checked.

If this was the final step:

1. State explicitly in the final response that the entire CodePlan is now complete (`CodePlan/<PlanName> 的所有步骤已完成`).
2. Ask whether to verify the whole plan (`是否需要对当前 CodePlan 进行校验？`) and wait for the user's answer before starting verification.

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
- verification performed or skipped
