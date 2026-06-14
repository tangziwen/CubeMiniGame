---
name: cp-verify
description: Verify implementation code against a specific CodePlan step, checking scope, idempotency state, changed files, completion criteria, and progress.md accuracy. Use when the user asks to review applied code, verify step implementation, audit a completed step, or check whether progress should be marked complete.
disable-model-invocation: true
---

# CodePlan Verify

## Purpose

Review implemented code against `design.md`, `progress.md`, and a specific `stepX.md`. This is code review for applied work, not planning review.

Do not modify code during verification unless the user explicitly asks.

## Plan Selection

When `CodePlan/` contains multiple plan directories, resolve the target before reviewing:

- If the user provides a directory name that exactly matches `CodePlan/<Name>`, use it directly.
- If the user says `latest` or `最新`, select the plan whose `design.md`, `progress.md`, or `step*.md` has the newest modification time.
- If there is only one plan directory, use it unless the user's request conflicts with it.
- If multiple plans exist and the target is ambiguous, ask the user to choose. Always include `latest/最新` as one option and explain which plan it currently resolves to.

## Required Reads

Read:

1. `AGENTS.md`
2. `design.md`
3. `progress.md`
4. the requested or inferred `stepX.md`
5. files listed in `文件操作清单`
6. relevant diffs or changed files

Read other steps only when the current step explicitly depends on them.

## Verification Focus

Prioritize:

- code that does not satisfy `完成判定`
- edits outside `文件操作清单`
- file operation lists that do not cover all actual changed files
- file operation lists that are too vague or incomplete to support `cp-sync`
- missing added files or stale deleted files
- partially applied changes marked complete
- future-step work accidentally included
- behavior that violates `需要保持不变的行为`
- clearly stale or missing CodeMap entries that should be reported for follow-up
- missing progress or verification notes
- build/test claims that were not actually run

## CodeMap Verification

Check whether the applied step should have affected existing CodeMap documents. CodeMap is incremental, so missing mapped `index.md` files are not automatically errors.

Report a CodeMap finding when:

- a navigation-worthy change added a feature/module, moved or relocated files/features/modules, split or merged modules, or deleted files/features/modules, but `cp-sync` was not run and an existing CodeMap stayed stale
- CodeMap was updated for an ordinary bug fix, local correctness fix, performance tweak, internal refactor without file/module movement, or implementation-detail-only change
- a relevant CodeMap `index.md` is missing and the change is navigation-worthy; phrase this as a recommendation to run `cm-full` backfill, not as a hard failure

Do not report CodeMap findings for ordinary bug fixes or implementation-only edits when the module navigation remains accurate.

Report CodeMap issues as findings only. Do not modify CodeMap during verification unless the user explicitly asks to run `cm-full`, `cm-light`, or `cp-sync`.

## Output

Lead with findings, ordered by severity, with file and line references when possible.

If no issues are found, say the implementation appears consistent with the step and list any residual risk.

Do not update `progress.md` unless the user explicitly asks you to reconcile status.
