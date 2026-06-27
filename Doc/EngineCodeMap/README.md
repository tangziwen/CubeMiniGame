# Engine Code Map

This directory is the lightweight navigation map for `CubeEngine/EngineSrc`.

Use these files to find engine entry points, capability boundaries, ownership contracts, and known traps. They are not a mirror of the source tree and do not replace source inspection.

## Writing Rule

- Keep one Markdown file per first-level `CubeEngine/EngineSrc` directory, with the same name as that directory.
- Do not create separate files for second-level directories. Fold them into the parent module file, for example `3D/Terrain` belongs in `3D.md`, and `BackEnd/vk` belongs in `BackEnd.md`.
- Keep entries concise and AI-facing: important objects, entry points, ownership/boundary rules, and known traps only.
- Do not document small private helpers unless they are a stable navigation anchor or a common source of bugs.
- Treat this directory as a code map, not generated API docs or a tutorial.

## Reading Rule

Read only the module file related to the current task. If the task spans multiple engine areas, start from `Doc/ProjectConventions.md`, then open the smallest set of module files needed.

## Module Files

- `2D.md`
- `3D.md`
- `Action.md`
- `AudioSystem.md`
- `BackEnd.md`
- `Base.md`
- `Collision.md`
- `doc.md`
- `Engine.md`
- `Event.md`
- `Font.md`
- `Game.md`
- `Interface.md`
- `Lighting.md`
- `Log.md`
- `Math.md`
- `Mesh.md`
- `Rendering.md`
- `Scene.md`
- `ScriptPy.md`
- `Shader.md`
- `Technique.md`
- `Texture.md`
- `Tina.md`
- `Utility.md`
