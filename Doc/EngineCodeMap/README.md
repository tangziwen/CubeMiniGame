# Engine Code Map

This directory is the lightweight navigation map for `CubeEngine/EngineSrc`.

Use it to choose which `CubeEngine/EngineSrc` directory, class, or file to open before reading source.

## Writing Rule

- CodeMap file names map one-to-one with first-level `CubeEngine/EngineSrc` directories.
- Fold subdirectories into the parent module file.
- Module files use `## Role` plus `## Important Objects`.
- Entries should name stable classes/files/systems and why they are useful search anchors.

## Reading Rule

Use the module list below to choose the smallest useful set of module files.

## Module Files

- `2D.md`: retained UI, sprites, labels, world billboards, notifications, ImGui bridge, and old GUI widgets.
- `3D.md`: 3D scene objects, terrain runtime, primitives, sky/shadow, particles, vegetation, and thumbnails.
- `Action.md`: time-based `Node` mutation actions and action sequencing.
- `AudioSystem.md`: audio playback wrappers, listener state, sound events, and one-shot sounds.
- `BackEnd.md`: window/platform bridge, backend abstractions, Vulkan backend objects, and legacy GL backend objects.
- `Base.md`: core object model and runtime services used across the engine.
- `Collision.md`: Bullet physics wrappers, rigid bodies, constraints, ray casts, and legacy ellipsoid collision.
- `doc.md`: legacy Doxygen-style source documentation stubs.
- `Engine.md`: runtime root, app lifecycle, frame update owner, worker jobs, and debug drawing.
- `Event.md`: input event queue, listener registration, capture ownership, and dispatch entry points.
- `Font.md`: FreeType font loading, glyph metrics, glyph atlas generation, and font cache.
- `Game.md`: engine-provided camera controllers, debug panels, and legacy/sample world scaffolding.
- `Interface.md`: shared renderable scene-object interfaces for 2D, 3D, scene culling, and renderer submission.
- `Lighting.md`: light data objects used by scenes and deferred lighting.
- `Log.md`: legacy/simple log facade.
- `Math.md`: vectors, matrices, quaternions, bounds, rays, planes, frustums, and rectangles.
- `Mesh.md`: geometry storage, GPU buffer submission, ray intersection, AABB calculation, and instancing payloads.
- `Rendering.md`: high-level render pipeline orchestration, render queues, post effects, and render-stage routing.
- `Scene.md`: scene lifetime, root traversal, spatial culling, picking, and render-command collection.
- `ScriptPy.md`: Python embedding bridge, script lifecycle, input forwarding, and binding helpers.
- `Shader.md`: shader program loading, macro injection, mutation variants, and backend shader collection creation.
- `Technique.md`: material templates, material instances, shader parameters, render-state identity, and material pools.
- `Texture.md`: CPU-side texture objects, texture cache, atlases, texture frames, and backend texture handles.
- `Tina.md`: custom scripting language tokenizer, parser, compiler, value model, and runtime VM.
- `Utility.md`: file/data helpers, JSON helpers, logging helpers, string utilities, math utilities, and stack trace support.
