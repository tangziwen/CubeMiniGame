# CubeEngine/EngineSrc/Technique

## Role

Material templates, material instances, shader parameters, render-state identity, and material/mesh pools.

## Important Objects

- `Material`: template loaded from material JSON. Owns techniques, default `ShadingParams`, texture slots, render state, source path, and full description string.
- `MaterialInstance`: per-object material facade. References a `Material`, owns or shares `ShadingParams`, supports instance overrides, clone/reload, and `ensureUniqueMaterial()`.
- `MaterialTechnique`: shader paths, `ShaderProgram`, render state, instancing/blend/depth/cull/topology/fill mode, and mutation/material flags.
- `ShadingParams`: uniform/texture name map used by materials and device descriptor updates.
- `TechniqueVar`: typed uniform value container.
- `MaterialPool`: named `MaterialInstance` and mesh cache.
- `MaterialState`: render-state enums and bit flags.
