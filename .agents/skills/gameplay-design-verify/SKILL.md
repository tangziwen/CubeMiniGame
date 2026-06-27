---
name: gameplay-design-verify
description: Explicitly verify CubeGame gameplay code against Doc/GameplayIntent design documents. Use only when the user asks to verify, audit, review, or check whether gameplay implementation matches accepted design intent. This is one-way verification: if code and intent disagree, report or fix code; never rewrite intent documents to match the current code unless the user separately asks for design sync.
---

# Gameplay Design Verify

## Purpose

Check whether gameplay implementation under `CubeEngine/Application/CubeGame` matches the accepted design intent in `Doc/GameplayIntent/`.

This skill is explicit only. It should not run merely because gameplay files changed.

## Required Reads

Read:

1. `AGENTS.md`
2. `Doc/GameplayIntent/README.md`
3. the relevant design docs under `Doc/GameplayIntent/`
4. the requested source files, changed files, or CodePlan step
5. only the source code needed to verify the design question

Do not scan unrelated gameplay files broadly.

## Scope Gate

- Verify only `CubeEngine/Application/CubeGame/` gameplay behavior and concepts.
- Ignore pure engine, renderer, backend, build, asset, or formatting changes unless they directly affect a gameplay concept described in `Doc/GameplayIntent/`.
- If the request is purely engine-side, say this skill does not apply.

## One-way Rule

`Doc/GameplayIntent/` is the accepted design source for this verification.

- If code conflicts with the design docs, treat the code as suspect.
- Do not update intent documents based on current code.
- If the design itself seems outdated, report that as an open question and recommend using `$gameplay-design-sync` only after the user confirms the design changed.
- If the user asks you to fix the mismatch, edit code or implementation-side docs, not intent docs.

## Verification Focus

Prioritize mismatches in:

- gameplay design intent
- concrete mechanisms under `Design`
- game concepts and aliases
- object relationships
- design boundaries
- thin `Concept Code Anchors` canonical code anchors

Do not report issues for harmless implementation details that preserve the design meaning.

## Output

Lead with findings, ordered by severity.

For each finding, include:

- design expectation
- code behavior or risk
- file/line reference when possible
- recommended code-side correction

If no issues are found, say the code appears consistent with the relevant gameplay intent and mention residual uncertainty.
