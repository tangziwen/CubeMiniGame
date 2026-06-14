---
name: cp-review
description: Review CodePlan documents, including explored requirements, design.md, progress.md, and step files. Use when the user asks to review a plan, design, refinement, step breakdown, progress state, or readiness before implementation.
disable-model-invocation: true
---

# CodePlan Review

## Shared Contract

Before acting, read:

1. `../_shared/codeplan/core.md`
2. `../_shared/codeplan/step-contract.md` when reviewing `stepX.md` or `progress.md`

## Purpose

Review CodePlan documents and planning quality. Do not review implementation code with this skill; use `cp-verify` for code-vs-step verification.

Prefer targeted review over whole-plan review, but support whole-plan review at design gates and milestones.

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
- `File Operations` lists that are missing, vague, incomplete, or too far below the top of a `stepX.md`
- insufficient preflight, completion, or duplicate-application checks
- roadmap order that may cause rework
- stale or inconsistent `progress.md`

When reviewing `stepX.md`, explicitly check `File Operations` against `step-contract.md`. Report any missing, vague, or incomplete `File Operations` list as a review finding.

## Output

Lead with findings, ordered by severity. Include file and line references when possible.

If there are no findings, say so clearly and mention residual risks or unverified areas.

Do not modify files during review unless the user explicitly asks.
