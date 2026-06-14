---
name: cp-refine
description: Refine a CodePlan design.md into precise step1.md through stepX.md files and progress.md. Use when the user asks to detail, refine, decompose, split, or prepare a CodePlan for implementation by weaker or cheaper models.
disable-model-invocation: true
---

# CodePlan Refine

## Shared Contract

Before acting, read:

1. `../_shared/codeplan/core.md`
2. `../_shared/codeplan/step-contract.md`

## Purpose

Turn `design.md` into precise, executable `stepX.md` files and a durable `progress.md` checklist. Steps must be detailed enough for a lower-capability model to execute with minimal guessing.

Do not implement source code while refining.

## Idempotency

Before generating or updating steps:

1. Check whether `step*.md` already exists.
2. If no steps exist, generate them.
3. If steps exist and `design.md` is not newer than any existing `step*.md`, tell the user steps already exist and ask whether to overwrite, append, or leave them unchanged.
4. If steps exist and `design.md` is newer than one or more `step*.md`, tell the user the steps may be stale and ask whether to regenerate all steps, update only affected steps, append new steps, or cancel.
5. Never silently overwrite completed step notes or `progress.md` summaries.

Compare modification times using `design.md` against all `step*.md`. Preserve user-written notes when possible.

## Workflow

1. Resolve the target CodePlan directory.
2. Read `design.md`.
3. Inspect existing `step*.md` and `progress.md` if present.
4. Apply the idempotency rules.
5. Refine coarse phases into concrete implementation steps.
6. Create or update `step1.md` through `stepX.md`.
7. Create or update `progress.md` with the step checklist, completed summaries, and blockers or notes.

Use the exact English headings and detail standard from `step-contract.md` for every generated step and `progress.md`.

Let agents infer the next step from the first unchecked item in `Step Status`.

When updating an existing plan, preserve completed summaries and manual notes.
