# CubeEngine/EngineSrc/Engine

## Role

Runtime root, app lifecycle, frame order, worker jobs, and debug drawing.

## Important Objects

- `Engine`: singleton-style runtime facade. Owns window size/config, render backend pointer, app delegate, frame timing, draw counters, and full update/render ordering.
- `AppEntry`: application callback boundary used by `Engine::run()` / `Engine::onStart()` / `Engine::update()`.
- `WorkerThreadSystem`: queued background jobs plus main-thread callbacks; `Engine::update()` pumps `mainThreadUpdate()`.
- `DebugSystem` / `DebugPrimitive`: immediate and retained debug geometry, wireframe overlay queue, and helper draw APIs.
- `EngineDef`: global engine version, render-device enum, reflection helpers, and common definitions.

## Flow

- `main()` should call `Engine::preSetting()` before `Engine::run()`.
- `Engine::run()` loads `config.json`, creates a `WindowBackEnd`, then enters the window loop.
- `Engine::onStart()` initializes log/render info/singletons/app/script/ImGui/audio.
- Frame order in `Engine::update()` is: renderer pre-tick, debug, physics, timers, worker main-thread callbacks, ImGui `NewFrame`, event key dispatch, frame listeners, app `onUpdate`, scene visit, pointer dispatch, audio update, render.

## Boundaries

- Application code enters through `AppEntry`; do not put gameplay in `Engine`.
- Rendering implementation is behind `RenderBackEndBase` / `GraphicsRenderer`; avoid spreading Vulkan details here.
- Input ownership belongs to `EventMgr`; `Engine` only schedules its phases.

## Known Traps

- Pointer events are intentionally dispatched after `SceneMgr::doVisit()` so node visibility/projection is current.
- `preSetting()` adds `./Asset/`, `./`, and `Asset.pkg` search paths; resource path bugs often start there.
- `WorkerThreadSystem::init()` is present but currently commented out in `onStart()`.
