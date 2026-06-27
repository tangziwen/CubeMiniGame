---
name: gameplay-design-sync
description: Assess whether completed CubeGame gameplay changes should be reconciled into Doc/GameplayIntent design documents. Use implicitly only after C/C++ source or header file changes under CubeEngine/Application/CubeGame, such as .c, .cc, .cpp, .cxx, .h, .hh, .hpp, .hxx, .inl, or .ipp, and only when the change may alter gameplay design intent, concrete design mechanisms, concepts, object relationships, aliases, or Concept Code Anchors. Ignore doc-only, build-only, asset-only, config-only, generated, formatting, and other non-code-file changes unless the user explicitly asks to evaluate gameplay design docs. First filter out pure engine/non-gameplay changes, then list candidate concept changes and ask the user before editing design docs. Do not use for pure engine changes, pure rendering/backend changes, ordinary bug fixes, helper/config/data-only additions, or implementation-only refactors that do not change gameplay concepts.
---

# Gameplay Design Sync

## Purpose

Assess whether `Doc/GameplayIntent/` should be updated after a completed change under `CubeEngine/Application/CubeGame`.

This skill is not a CodeMap sync. It records designer intent, game concepts, concept aliases, object relationships, and very thin `Concept Code Anchors`.

Concepts are designer/gameplay concepts, not a code type inventory. A concept may map to one canonical code anchor, but many code types should have no design concept entry.

## Hard Scope Gate

Before editing anything, inspect the changed-file list from conversation context, `git diff --name-only`, or the relevant CodePlan `File Operations`.

- Treat only C/C++ source or header files as trigger files: `.c`, `.cc`, `.cpp`, `.cxx`, `.h`, `.hh`, `.hpp`, `.hxx`, `.inl`, and `.ipp`.
- For implicit post-change use, if there are no changed trigger files under `CubeEngine/Application/CubeGame/`, stop and report that no gameplay intent sync is needed.
- If the user explicitly asks to evaluate or update gameplay design docs, the skill may still run without trigger files, but treat non-code changes as supporting context and classify `skip` unless they clearly express a gameplay design change.
- Ignore changed files with other extensions, including docs, build files, assets, generated files, scripts, config, shaders, and data files, when deciding whether the skill should run implicitly.
- If changes are pure engine, renderer, backend, asset, build, formatting, or bug-fix work, stop unless the user explicitly says the gameplay design changed.
- Ignore changes outside `CubeEngine/Application/CubeGame/` when deciding design-doc content.
- Do not update `Doc/GameplayIntent/` for implementation-only refactors that preserve the same gameplay concepts.
- Do not propose a new concept merely because a new code type exists.
- Treat helper, config, data, view, visual, manager, and other auxiliary types as non-concepts by default unless the user request or design text makes them gameplay concepts.
- Allow implementation-adjacent concepts when they directly participate in gameplay mechanisms, player actions, or design semantics.

Be conservative. No update is better than recording a transient or accidental design.

After the hard scope gate, do not edit design docs immediately. First summarize the candidate design changes, list any proposed new or changed concepts, and ask the user whether to sync them.

## Required Reads

Read:

1. `AGENTS.md`
2. `Doc/GameplayIntent/README.md`
3. the relevant existing design docs in `Doc/GameplayIntent/`
4. the completed change context: user request, CodePlan docs when present, changed-file list, and only the source files needed to confirm concept-level changes

Do not scan unrelated gameplay files broadly.

## Update Rules

Update only long-lived design facts:

- new or changed gameplay design intent
- new or changed concrete design mechanisms under a `Design` section
- new, renamed, merged, or split game concepts
- changed object relationships or design boundaries
- new concept aliases
- changed thin concept-to-code-anchor mapping in `Concept Code Anchors`

Do not write:

- function flow
- implementation strategy
- class internals
- directory mirrors
- complete file lists
- temporary implementation notes
- validation details

Concept names may have aliases, but `Concept Code Anchors` should list only canonical code anchors that exist in the current code.

Do not add helper/config/data-only types to `Game Concepts` or `Concept Code Anchors` unless they are explicitly part of the gameplay design vocabulary.

If a code type or structure has no direct design concept but is critical for TD-level orientation, especially important `XXXSystem`, manager, or helper types, put it in `Critical Code Anchors` instead of `Game Concepts` or `Concept Code Anchors`.

Use the document heading order from `Doc/GameplayIntent/README.md`: `Design Intent`, `Experience`, `Design`, `Game Concepts`, `Concept Code Anchors`, `Design Boundaries`. `GameplayDesign.md` may also contain `Code Rules` for broad gameplay code organization principles. Documents may contain `Critical Code Anchors` for important non-concept system/manager/helper names. Do not create a separate `Non-goals` heading.

## Reconcile Workflow

1. Classify the change as `skip`, `needs-update`, or `blocked`.
2. If `skip`, explain briefly and stop.
3. If `blocked`, ask for the missing design decision.
4. If `needs-update`, summarize the proposed design-doc update, including the exact concepts or aliases to add/change, and ask the user for explicit confirmation.
5. Only after confirmation, patch the smallest relevant design doc.
6. Keep headings stable and preserve existing accepted design text unless the completed change replaces it.
7. Run `git diff --check` for edited design docs.

## Output

End with:

- classification and reason
- gameplay files considered
- proposed concepts or aliases, if any
- whether user confirmation was requested or received
- design docs updated, if any
- confirmation that pure non-gameplay changes were ignored
- verification performed
