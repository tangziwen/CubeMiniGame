---
name: cp-refine
description: Refine a CodePlan design.md into precise step1.md through stepX.md files and progress.md. Use when the user asks to detail, refine, decompose, split, or prepare a CodePlan for implementation by weaker or cheaper models.
disable-model-invocation: true
---

# CodePlan Refine

## Purpose

Turn `design.md` into precise, executable `stepX.md` files and a durable `progress.md` checklist. Steps must be detailed enough for a lower-capability model to execute with minimal guessing.

Do not implement source code while refining.

## Plan Selection

When `CodePlan/` contains multiple plan directories, resolve the target before editing:

- If the user provides a directory name that exactly matches `CodePlan/<Name>`, use it directly.
- If the user says `latest` or `最新`, select the plan whose `design.md`, `progress.md`, or `step*.md` has the newest modification time.
- If there is only one plan directory, use it unless the user's request conflicts with it.
- If multiple plans exist and the target is ambiguous, ask the user to choose. Always include `latest/最新` as one option and explain which plan it currently resolves to.

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

## Step File Shape

Every `stepX.md` must begin with a concrete file operation list within the first 30 to 50 lines so `cp-apply`, `cp-review`, and `cp-sync` can find it quickly:

```md
# Step N: <Short Action Title>

## 文件操作清单

### 编辑
- path/to/file.cpp

### 新增
- path/to/new_file.h

### 删除
- path/to/old_file.cpp

### 移动
- old/path.h -> new/path.h

## 前置校验

## 目标

## 需要保持不变的行为

## 具体实施顺序

## 禁止顺手做的事

## 完成判定

## 验证方式

## 部分应用/重复应用处理
```

The file operation list is a hard requirement:

- include all four groups: `编辑`, `新增`, `删除`, `移动`
- if no files belong in a category, write `无`
- list exact file paths only; do not use directory-wide globs or vague placeholders such as `相关文件`, `若干文件`, or `按需修改`
- write moves as `old/path -> new/path`
- if a move is uncertain, express it as delete plus add
- make the list precise enough for `cp-sync` to infer the affected source directories without guessing

## Detail Standard

Assume `cp-apply` may be run by a cheaper or weaker model. Therefore every step must be precise:

- name all files to edit, add, delete, or move
- keep the file operation list complete and precise enough to support `cp-sync`
- specify preconditions and expected existing symbols
- describe exact order of edits
- state what behavior must remain unchanged
- state what must not be done in this step
- define completion criteria and lightweight verification
- describe how to detect already-applied or partially-applied work

Avoid vague directions such as "clean up dependencies" without naming the files and target state.

Use adaptive precision instead of writing every step like a line-by-line transcript:

- For difficult edits, include exact class names, function signatures, field names, enum names, config keys, and current search anchors.
- For especially fragile edits, include an approximate line number as a secondary hint, but never make the line number the only way to find the code. Pair it with a symbol, nearby heading, or unique text snippet.
- For lifecycle, ownership, render-pipeline, resource-management, serialization, or cross-module behavior, spell out the expected before/after relationship in more detail.
- For simple local edits, describe the intended target state and affected symbols without excessive mechanical instructions.
- Do not treat the lower-capability executor as unable to reason. Give enough precision to avoid guessing, but avoid verbose "click-by-click" or "change this line, then the next line" narration when the surrounding code is straightforward.
- If a step depends on a subtle invariant, name the invariant explicitly in `需要保持不变的行为` or `禁止顺手做的事`.

## Progress File

Create `progress.md` next to `design.md`:

```md
# Progress

## Step 状态

- [ ] Step 1: <title>
- [ ] Step 2: <title>

## 已完成摘要

暂无。

## 当前阻塞/注意事项

- 不要自行编译项目，除非用户明确要求。
```

Let agents infer the next step from the first unchecked item in `Step 状态`.

When updating an existing plan, preserve completed summaries and manual notes.
