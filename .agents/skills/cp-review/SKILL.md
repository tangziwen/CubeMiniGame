---
name: cp-review
description: Review CodePlan documents, including explored requirements, design.md, progress.md, and step files. Use when the user asks to review a plan, design, refinement, step breakdown, progress state, or readiness before implementation.
---

# CodePlan Review

## Purpose

Review CodePlan documents and planning quality. Do not review implementation code with this skill; use `cp-verify` for code-vs-step verification.

Prefer targeted review over whole-plan review, but support whole-plan review at design gates and milestones.

## Plan Selection

When `CodePlan/` contains multiple plan directories, resolve the target before reviewing:

- If the user provides a directory name that exactly matches `CodePlan/<Name>`, use it directly.
- If the user says `latest` or `最新`, select the plan whose `design.md`, `progress.md`, or `step*.md` has the newest modification time.
- If there is only one plan directory, use it unless the user's request conflicts with it.
- If multiple plans exist and the target is ambiguous, ask the user to choose. Always include `latest/最新` as one option and explain which plan it currently resolves to.

## Scope Choice

Use the smallest scope that answers the user:

- review `design.md` before refinement
- review one `stepX.md` before apply
- review `progress.md` when status looks inconsistent
- review all step headings after `cp-refine`
- review the whole plan only when requested or at a phase boundary

## Review Focus

Prioritize:

- unclear requirements
- missing constraints or non-goals
- architecture contradictions
- steps too large for one agent context
- file operation lists that are missing, vague, incomplete, or too far below the top of a `stepX.md`
- insufficient preflight, completion, or duplicate-application checks
- roadmap order that may cause rework
- stale or inconsistent `progress.md`

When reviewing `stepX.md`, explicitly check the file operation list:

- appears within the first 30 to 50 lines
- includes all four groups: `编辑`, `新增`, `删除`, `移动`
- writes `无` for every empty group
- names exact file paths rather than directory globs or vague placeholders such as `相关文件`, `若干文件`, or `按需修改`
- writes moves as `old/path -> new/path`, or uses delete plus add when a move is uncertain
- is precise enough for `cp-sync` to infer the affected source directories

Report any missing, vague, or incomplete file operation list as a review finding.

## Output

Lead with findings, ordered by severity. Include file and line references when possible.

If there are no findings, say so clearly and mention residual risks or unverified areas.

Do not modify files during review unless the user explicitly asks.
