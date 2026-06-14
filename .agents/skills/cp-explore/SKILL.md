---
name: cp-explore
description: Explore and clarify a prospective CodePlan before writing design.md. Use when the user has a rough requirement and wants read-only investigation, clarifying questions, constraints discovery, and a compact candidate CodePlan name before running cp-plan.
disable-model-invocation: true
---

# CodePlan Explore

## Purpose

Clarify the user's intent before creating or updating a CodePlan. This is the read-only "grill me" phase.

Do not create, edit, delete, move, format, stage, commit, or generate files while using this skill. Only read files and ask or answer questions.

## Workflow

1. Read project instructions such as `AGENTS.md`.
2. Restate the user's rough requirement in one or two sentences.
3. Inspect only the repository areas needed to understand the problem.
4. Identify existing `CodePlan/` directories that may be related.
5. Ask focused clarifying questions when the plan would otherwise be ambiguous.
6. Suggest one compact PascalCase CodePlan directory name.
7. When the requirement is clear enough, tell the user the next step is `cp-plan`.

## Read-Only Rules

Allowed:

- list files
- read files
- search with `rg`
- inspect git status or diffs
- summarize architecture and risks

Forbidden:

- editing any file
- creating `CodePlan/<Name>`
- updating `design.md`
- updating `progress.md`
- running formatters
- compiling or building

If the user asks to make changes during exploration, stop and recommend switching to the appropriate next skill.

## Questions

Ask only questions that materially affect the plan. Prefer a short list.

Good questions identify:

- desired behavior or final architecture
- hard constraints and non-goals
- risky compatibility expectations
- whether old code should be preserved or removed
- whether the work should be phased for cheaper models

## Output

End with:

- a concise requirement summary
- key constraints and non-goals
- open questions, if any
- candidate CodePlan name
- whether the user should proceed to `cp-plan`

Do not write a design document in this phase.
