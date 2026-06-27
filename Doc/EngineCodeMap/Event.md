# CubeEngine/EngineSrc/Event

## Role

Central input event queue, dispatch order, capture ownership, and per-frame listener updates.

## Important Objects

- `EventMgr`: queues key/mouse/scroll events, sorts listeners, dispatches capture/node/standalone listeners, and records consumed mouse buttons.
- `EventListener`: override surface for key, char, mouse, scroll, and frame update callbacks.
- `EventInfo`: queued event payload.
- `Event.h` key/mouse constants: GLFW-like key code definitions used across engine/game code.
