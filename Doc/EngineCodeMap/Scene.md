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
