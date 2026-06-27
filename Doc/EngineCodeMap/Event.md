# CubeEngine/EngineSrc/Event

## Role

Central input event queue, dispatch order, capture ownership, and per-frame listener updates.

## Important Objects

- `EventMgr`: queues key/mouse/scroll events, sorts listeners, dispatches capture/node/standalone listeners, and records consumed mouse buttons.
- `EventListener`: override surface for key, char, mouse, scroll, and frame update callbacks.
- `EventInfo`: queued event payload.
- `Event.h` key/mouse constants: GLFW-like key code definitions used across engine/game code.

## Flow

- Platform input calls `handleKeyPress`, `handleMousePress`, etc.; these only enqueue.
- Current frame flow is split: `beginFrame()`, `dispatchQueuedKeyEvents()`, `updateFrameListeners()`, scene visit, then `dispatchQueuedPointerEvents()`.
- Capture listener receives first chance and can swallow input.
- Node listeners are rebuilt by visiting visible scene nodes and sorted by `Node::getNodeEventPriority()` descending.
- Standalone listeners are sorted by `EventListener::getStandaloneEventPriority()` descending.

## Contracts

- Return `true` plus `isSwallow()==true` to stop propagation.
- `isMouseButtonConsumed(button)` is the cross-system way to gate world/editor clicks after UI capture.
- Register node-attached listeners with `addNodeEventPriorityListener(node, listener)` when hit priority should follow scene visibility.

## Known Traps

- Pointer dispatch intentionally waits until after scene visit; do not move it earlier unless projected UI/node visibility is redesigned.
- Direct ImGui polling in gameplay/editor draw code can disagree with `EventMgr` ownership; prefer listener routing.
