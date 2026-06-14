---
name: cp-plan
description: Create or update a CodePlan design.md from clarified requirements, repository observations, constraints, and architecture intent. Use after cp-explore or when the user explicitly asks to write, update, or refine the high-level CodePlan design.
disable-model-invocation: true
---

# CodePlan Plan

## Shared Contract

Before acting, read `../_shared/codeplan/core.md`.

## Purpose

Create or update `CodePlan/<PlanName>/design.md`. Capture durable requirements, constraints, current state, target architecture, coarse phases, risks, and validation strategy.

Do not create `stepX.md` files or implement code while planning unless the user explicitly changes the task.

## Idempotency

If the target plan already has `design.md`, do not silently overwrite it.

Before updating an existing `design.md`:

1. Summarize what already exists.
2. Ask what the user wants to change, clarify, or extend.
3. Preserve stable design decisions unless the user explicitly replaces them.
4. If `step*.md` already exists, warn that updating `design.md` may make the steps stale and should be followed by `cp-refine`.

## Readiness Gate

Before creating or updating `design.md`, verify that the request is clear enough to become a CodePlan.

Do not write `design.md` when the user's requirement is only a short, broad, or ambiguous wish such as "help me implement a spaceship simulator" or "make the engine better".

A request is not ready when important information is missing, such as:

- target behavior or user-visible result
- affected subsystem or likely repository area
- hard constraints, compatibility expectations, or non-goals
- whether old behavior should be preserved, replaced, or deprecated
- enough context to estimate the CodePlan scope

If the request is not ready:

1. Stop before creating or editing `design.md`.
2. Ask only the clarifying questions that materially affect the plan.
3. Recommend using `cp-explore` when the user needs discovery before planning.
4. Offer a compact candidate CodePlan name only if enough direction is already visible.

## Workflow

1. Read project instructions, especially `AGENTS.md`.
2. Resolve the target CodePlan directory.
3. Inspect related existing CodePlan documents for style and continuity.
4. Gather requirements, constraints, open questions, and relevant repository facts.
5. Apply the readiness gate and stop if the request is not clarified enough.
6. Apply the shared scope gate from `core.md` and stop with split suggestions if the plan is too large.
7. Create or update `design.md`.
8. Keep implementation details at architecture level.
9. Leave file-by-file execution details for `cp-refine`.

## Design Shape

Use this exact English heading shape by default:

```md
# <Readable Plan Title>

## Requirements

## Notes

## Current State

## Core Problem

## Target Architecture

## Overall Route

## Phases

## Key Risks

## Validation Strategy

## Validation Status
```

Coarse architecture-level phases belong in `design.md`. Detailed file operations belong in `stepX.md`.

The last phase in `Phases` must always be a validation phase. By default, validation is text-level or static verification: review the design against requirements and non-goals, inspect code symbols and paths, verify planned file scope, and check that later `stepX.md` files stay within the plan. Do not make compilation the default validation method unless the user explicitly asks for it.

## Context Budget

Write `design.md` so implementation agents only need:

- `design.md`
- `progress.md`
- the current `stepX.md`

Avoid requiring all step files to understand the overall design.

Do not instruct agents to compile unless the user explicitly asks.
