---
name: cp-apply
description: Apply exactly one CodePlan step to the repository, with preflight idempotency checks and progress.md updates. Use when the user asks to execute, apply, continue, or implement a specific step or the next unchecked step.
---

# CodePlan Apply

## Purpose

Implement exactly one `stepX.md` at a time. Run preflight checks first so repeated or partially-applied steps are detected before editing.

## Required Reads

Before editing, read:

1. `AGENTS.md`
2. `CodePlan/<PlanName>/design.md`
3. `CodePlan/<PlanName>/progress.md`
4. the requested or inferred `stepX.md`

Read other step files only when the current step explicitly depends on them.

## Plan Selection

When `CodePlan/` contains multiple plan directories, resolve the target before reading or editing:

- If the user provides a directory name that exactly matches `CodePlan/<Name>`, use it directly.
- If the user says `latest` or `最新`, select the plan whose `design.md`, `progress.md`, or `step*.md` has the newest modification time.
- If there is only one plan directory, use it unless the user's request conflicts with it.
- If multiple plans exist and the target is ambiguous, ask the user to choose. Always include `latest/最新` as one option and explain which plan it currently resolves to.

## Step Selection

- If the user names `stepN`, use that step.
- Otherwise use the first unchecked item in `progress.md`.
- If the step is already checked, stop and ask whether to verify, rerun, or choose another step.
- If every step in `progress.md` is already checked, the plan is fully applied. Stop and explicitly tell the user that all tasks for this CodePlan are complete (`当前 CodePlan 的所有任务已经完成`). Ask for their intent, for example:
  - Does something look wrong and they want to rerun or verify a specific step?
  - Do they want to switch to another CodePlan?
  - Do they want to run whole-plan verification?
  - Do they want to mark the plan as incomplete and continue editing?
  - Do not silently proceed or auto-pick another plan.

## Preflight Idempotency

Before editing, classify the step state as one of:

- `未开始`
- `部分应用`
- `已完成`
- `冲突`

Use the `文件操作清单`, `前置校验`, `完成判定`, and `部分应用/重复应用处理` sections from `stepX.md`.

Minimum checks:

- edited files exist
- added files do not already exist unless the step says they may
- deleted files still exist unless already removed by this step
- moved files are treated as source deletion plus destination addition
- expected symbols, CMake targets, functions, classes, or includes are present or absent as described by the step
- `progress.md` checkbox state matches observed repository state

If the work appears fully applied but unchecked, do not edit code. Update `progress.md` only if the user asked you to reconcile status, otherwise report the mismatch.

If the work is partially applied, explain the observed partial state before continuing. Ask for confirmation when continuing could overwrite uncertain user work.

## Apply Rules

- Implement only the current step.
- Do not do future-step work.
- Do not perform opportunistic refactors.
- Do not compile or build this repository unless the user explicitly asks.
- Do not revert user changes.
- Do not run destructive git commands.

## Progress Updates

After completing a step, update `progress.md`:

```md
## Step 状态

- [x] Step N: <title>

## 已完成摘要

### Step N

完成时间：YYYY-MM-DD
主要改动：
- ...

验证状态：
- 未编译；按项目规则避免自行编译。

遗留问题：
- ...
```

Leave the step unchecked if blocked or only partially applied. Record the blocker under `当前阻塞/注意事项`.

After completing a step, also infer which source directories may need CodeMap follow-up from the step's `文件操作清单`:

- map edited and added files to their containing source directories
- map deleted files to their former containing source directories
- map moved files to both the old and new containing source directories
- suggest the corresponding CodeMap paths when they can be derived from those directories
- do not run `cp-sync` automatically unless the user explicitly asks

## Plan Completion

After marking the current step complete, check whether it is the last unchecked step listed in `progress.md` (i.e., all steps are now checked).

If this was the final step:

1. State explicitly in the final response that the entire CodePlan is now complete (`CodePlan/<PlanName> 的所有步骤已完成`).
2. Ask the user whether they want to run verification for the whole plan (`是否需要对当前 CodePlan 进行校验？`).
3. Do not start any verification on your own; wait for the user's answer.

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
- possible follow-up `cp-sync` directories or CodeMap paths inferred from the current step
- whether `cp-sync` was intentionally not run
- verification performed or skipped
