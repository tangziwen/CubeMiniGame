---
name: cp-plan
description: Create or update a CodePlan design.md from clarified requirements, repository observations, constraints, and architecture intent. Use after cp-explore or when the user explicitly asks to write, update, or refine the high-level CodePlan design.
---

# CodePlan Plan

## Purpose

Create or update `CodePlan/<PlanName>/design.md`. Capture durable requirements, constraints, current state, target architecture, coarse phases, risks, and validation strategy.

Do not create `stepX.md` files or implement code while planning unless the user explicitly changes the task.

## Plan Selection

When `CodePlan/` contains multiple plan directories, resolve the target before writing:

- If the user provides a directory name that exactly matches `CodePlan/<Name>`, use it directly.
- If the user says `latest` or `最新`, select the plan whose `design.md`, `progress.md`, or `step*.md` has the newest modification time.
- If the request clearly creates a new plan and no exact existing name was given, choose a new compact PascalCase name.
- If the request might update an existing plan and the target is ambiguous, ask the user to choose. Always include `latest/最新` as one option and explain which plan it currently resolves to.

## Idempotency

If the target plan already has `design.md`, do not silently overwrite it.

Before updating an existing `design.md`:

1. Summarize what already exists.
2. Ask what the user wants to change, clarify, or extend.
3. Preserve stable design decisions unless the user explicitly replaces them.
4. If `step*.md` already exists, warn that updating `design.md` may make the steps stale and should be followed by `cp-refine`.

When the user asks to plan `latest` or `最新`, treat it as updating an existing plan and ask what area should be refined unless the requested edit is already specific.

## Workflow

1. Read project instructions, especially `AGENTS.md`.
2. Resolve the target CodePlan directory.
3. Inspect related existing CodePlan documents for style and continuity.
4. Gather requirements, constraints, open questions, and relevant repository facts.
5. Create or update `design.md`.
6. Keep implementation details at architecture level.
7. Leave file-by-file execution details for `cp-refine`.

## Design Shape

Use this shape by default:

```md
# <Readable Plan Title>

## 需求

## 注意事项

## 当前状态

## 核心问题

## 目标架构

## 建议总体路线

## 阶段划分

## 关键风险

## 验证策略

## 当前验证状态
```

Coarse architecture-level phases belong in `design.md`. Detailed file operations belong in `stepX.md`.

## Context Budget

Write `design.md` so implementation agents only need:

- `design.md`
- `progress.md`
- the current `stepX.md`

Avoid requiring all step files to understand the overall design.

## Project Conventions

For this repository:

- Use UTF-8 for markdown files.
- Preserve Chinese wording when it is clearer.
- Remember this is a single-person C++ engine project and Vulkan is the default renderer.
- Do not instruct agents to compile unless the user explicitly asks.
