# CubeEngine/EngineSrc/Interface

## Role

Renderable scene-object interfaces shared by 2D, 3D, scene culling, and renderer submission.

## Important Objects

- `Drawable`: `Node` plus material, camera, color/overlay color, render priority, and render-command setup hooks.
- `Drawable2D`: content size, anchor point, screen/local AABB, alpha, off-screen check, and 2D transform behavior.
- `Drawable3D`: local/world AABB, hit tests, octree index, drawable/render-stage flags, outline settings, instanced-data hook, and mesh access.
- `Drawable3DGroup`: helper for ray/AABB/sphere checks over multiple drawables.
- `DepthPolicy`: per-command depth enable/test mode.
