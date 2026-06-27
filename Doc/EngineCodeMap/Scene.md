# CubeEngine/EngineSrc/Scene

## Role

Scene lifetime, root traversal, spatial culling, picking, and render-command collection.

## Important Objects

- `Scene`: owns root node, default 3D/GUI cameras, ambient/directional light, skybox, octree scene, debug/console panels, and direct draw list.
- `SceneMgr`: current-scene singleton and `doVisit()` entry.
- `SceneCuller`: collects render commands into common and CSM render queues.
- `OctreeScene`: spatial index for `Drawable3D` culling, ray hit tests, and range queries.
- `ScenePickingSystem`: lightweight registered pick targets with category mask, priority, payload, and shape.
- `BoxPickShape`: local AABB plus world transform pick shape.

## Flow

- Add objects through `Scene::addNode()` or the scene root.
- `SceneMgr::doVisit()` drives scene traversal; render and event systems assume it has run for current-frame visibility.
- `SceneCuller::collectPrimitives()` reads visible scene state and fills `RenderQueue`s for renderer stages.

## Boundaries

- `Scene` owns global scene services; gameplay systems should attach objects to root/layers rather than bypassing traversal.
- Use `ScenePickingSystem` for semantic editor/game picks that are not naturally `Drawable3D` ray hits.

## Known Traps

- `OctreeScene` is scene culling/picking infrastructure, not the terrain LOD octree.
- `SceneCuller` is still mostly common-stage centric; stage-aware behavior should be verified in source before relying on it.
