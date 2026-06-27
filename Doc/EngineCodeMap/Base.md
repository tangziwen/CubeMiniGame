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

## Contracts

- `Node` derived scene objects usually follow two-phase construction with `create()` helpers.
- `Node::visit()` builds direct draw lists and triggers logical traversal; scene and event systems depend on visibility and priority being current.
- `Camera::setOffsetPixel()` is used by temporal AA jitter; reset it when disabling TSAA.

## Boundaries

- Keep `Base` free of gameplay concepts.
- Use `Node` for scene lifetime only when the object participates in transform/tree/update/render behavior.

## Known Traps

- `Node::setLocalPriority()` affects child ordering; `setNodeEventPriority()` affects pointer dispatch order.
- `Node` children are raw pointers owned by the scene tree conventions, not smart-pointer ownership.
