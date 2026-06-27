# CubeEngine/EngineSrc/Engine

## Role

Runtime root, app lifecycle, frame order, worker jobs, and debug drawing.

## Important Objects

- `Engine`: singleton-style runtime facade. Owns window size/config, render backend pointer, app delegate, frame timing, draw counters, and full update/render ordering.
- `AppEntry`: application callback boundary used by `Engine::run()` / `Engine::onStart()` / `Engine::update()`.
- `WorkerThreadSystem`: queued background jobs plus main-thread callbacks; `Engine::update()` pumps `mainThreadUpdate()`.
- `DebugSystem` / `DebugPrimitive`: immediate and retained debug geometry, wireframe overlay queue, and helper draw APIs.
- `EngineDef`: global engine version, render-device enum, reflection helpers, and common definitions.
