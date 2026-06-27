# CubeEngine/EngineSrc/Base

## Role

Core object model and low-level runtime services used by most engine modules.

## Important Objects

- `Node`: scene-tree base object. Owns transform, parent/children, visibility, local priority, event priority, scene pointer, and action updates.
- `Camera`: `Node` with projection/view/frustum, screen/world conversion, jitter pixel offset, and frustum updates.
- `Singleton<T>`: lazy singleton helper used by many systems.
- `Timer` / `TimerMgr`: timer objects pumped from `Engine::update()`.
- `GUIDMgr` / `GuidObj`: string GUID registry for object lookup.
- `TranslationMgr`: language table accessor behind `TR(...)`.
- `BlackBoard`: simple shared key/value state store.
