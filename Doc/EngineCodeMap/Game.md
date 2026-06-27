# CubeEngine/EngineSrc/Game

## Role

Engine-provided camera controllers, debug panels, and legacy/sample world scaffolding.

## Important Objects

- `FPSCamera`: first-person camera with keyboard/mouse input, optional Bullet character controller, gravity, collision callback, and global input block.
- `EditorCamera`: editor/city-builder camera with free/orbit-like controls, focus arm, scroll zoom, and separate mouse/input block flags.
- `OrbitCamera`: focus-node orbit camera with zoom and optional collision/gravity legacy behavior.
- `ConsolePanel`: ImGui console with command history and log output.
- `DebugInfoPanel`: ImGui runtime stats panel for FPS, draw calls, timing, vertices, and scene node count.
- `GameCamera`, `WorldSystem`, `CubePlaceHolder`: legacy/sample scaffolding under `TDS`.

## Contracts

- Camera controllers are both `Camera` and `EventListener`; register them with `EventMgr` when they should receive input.
- Use static input-block flags to suppress camera controls while editor/UI modes own input.

## Boundaries

- This directory is engine-side convenience/game scaffolding, not `CubeGame` application logic.
- Gameplay-specific systems belong under `CubeEngine/Application/CubeGame`.

## Known Traps

- `FPSCamera` can run with or without physics; check `isIsOpenPhysics()` before assuming Bullet character state exists.
