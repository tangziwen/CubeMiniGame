---
name: cp-verify
description: Verify implementation code against a specific CodePlan step, checking scope, idempotency state, changed files, completion criteria, and progress.md accuracy. Use when the user asks to review applied code, verify step implementation, audit a completed step, or check whether progress should be marked complete.
disable-model-invocation: true
---

# CodePlan Verify

## Shared Contract

Before acting, read:

1. `../_shared/codeplan/core.md`
2. `../_shared/codeplan/step-contract.md`

## Purpose

Review implemented code against `design.md`, `progress.md`, and a specific `stepX.md`. This is code review for applied work, not planning review.

Do not modify code during verification unless the user explicitly asks.

## Required Reads

Read:

1. `AGENTS.md`
2. `design.md`
3. `progress.md`
4. the requested or inferred `stepX.md`
5. files listed in `File Operations`
6. relevant diffs or changed files

Read other steps only when the current step explicitly depends on them.

## Verification Focus

Prioritize:

- code that does not satisfy `Completion Criteria`
- edits outside `File Operations`
- `File Operations` lists that do not cover all actual changed files
- `File Operations` lists that are too vague or incomplete to support reliable scope verification
- missing added files or stale deleted files
- partially applied changes marked complete
- future-step work accidentally included
- behavior that violates `Preserved Behavior`
- missing progress or verification notes
- build/test claims that were not actually run

## Output

Lead with findings, ordered by severity, with file and line references when possible.

If no issues are found, say the implementation appears consistent with the step and list any residual risk.

Do not update `progress.md` unless the user explicitly asks you to reconcile status.
