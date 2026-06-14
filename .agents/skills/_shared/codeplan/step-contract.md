# CodePlan Step Contract

This contract defines the shared `stepX.md` and `progress.md` rules used by CodePlan skills.

## Step Selection

- If the user names `stepN`, use that step.
- Otherwise use the first unchecked item in `progress.md`.
- If multiple steps could apply or the choice is ambiguous, ask the user before proceeding.
- If the step is already checked, stop and ask whether to verify, rerun, or choose another step.
- If every step in `progress.md` is already checked, the plan is fully applied. Stop and explicitly tell the user that all tasks for this CodePlan are complete (`当前 CodePlan 的所有任务已经完成`) and ask for their intent.
- Do not silently proceed or auto-pick another plan when the current plan is already complete.

## Step File Shape

Every `stepX.md` must use these exact English protocol headings and must begin with a concrete `File Operations` list within the first 30 to 50 lines:

```md
# Step N: <Short Action Title>

## File Operations

### Edit
- path/to/file.cpp

### Add
- path/to/new_file.h

### Delete
- path/to/old_file.cpp

### Move
- old/path.h -> new/path.h

## Preconditions

## Goal

## Preserved Behavior

## Implementation Order

## Out of Scope

## Completion Criteria

## Verification

## Idempotency Handling
```

## File Operation List

The `File Operations` section is a hard requirement:

- Include all four groups: `Edit`, `Add`, `Delete`, `Move`.
- If no files belong in a category, write `None`.
- List exact file paths only.
- Do not use directory-wide globs, wildcards, or vague placeholders such as `相关文件`, `若干文件`, or `按需修改`.
- Write moves as `old/path -> new/path`.
- If a move is uncertain, express it as delete plus add.
- Keep the list precise enough for downstream scope checks without guessing.

## Detail Standard

Assume `cp-apply` may be run by a cheaper or weaker model. Each step must be precise enough to avoid guessing:

- Name all files to edit, add, delete, or move.
- Specify preconditions and expected existing symbols.
- Describe the exact order of edits.
- State what behavior must remain unchanged.
- State what must not be done in this step.
- Define completion criteria and lightweight verification.
- Describe how to detect already-applied or partially-applied work.

Use adaptive precision:

- For difficult edits, include exact symbols, config keys, and search anchors.
- For fragile edits, use approximate line numbers only as secondary hints paired with symbols or unique text.
- For lifecycle, ownership, render-pipeline, resource-management, serialization, or cross-module behavior, spell out the before/after relationship.
- For simple local edits, describe the target state and affected symbols without mechanical narration.
- Put subtle invariants in `Preserved Behavior` or `Out of Scope`.

## Preflight State

Before applying a step, classify it as one of:

- `not_started`
- `partially_applied`
- `complete`
- `conflict`

Use the `File Operations`, `Preconditions`, `Completion Criteria`, and `Idempotency Handling` sections from `stepX.md`.

Minimum checks:

- edited files exist
- added files do not already exist unless allowed
- deleted files still exist unless already removed by this step
- moved files are checked as source deletion plus destination addition
- expected symbols, CMake targets, functions, classes, or includes match the step
- `progress.md` checkbox state matches repository state

If work appears fully applied but unchecked, do not edit code. Update `progress.md` only if the user asked to reconcile status; otherwise report the mismatch.

If work is partially applied, explain the observed partial state before continuing. Ask for confirmation when continuing could overwrite uncertain user work.

## Progress File

Create `progress.md` next to `design.md`:

```md
# Progress

## Step Status

- [ ] Step 1: <title>
- [ ] Step 2: <title>

## Completed Summary

None yet.

## Current Blockers / Notes

- Do not compile or build unless the user explicitly asks.
```

After completing a step, update `progress.md` with the checked status and a concise completion summary:

```md
## Step Status

- [x] Step N: <title>

## Completed Summary

### Step N

Completed At: YYYY-MM-DD
Changes:
- ...

Verification Status:
- Not compiled; project rules say to avoid compiling unless explicitly requested.

Remaining Issues:
- ...
```

Leave the step unchecked if blocked or only partially applied. Record blockers under `Current Blockers / Notes`.
