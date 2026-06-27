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

## Contracts

- Shared template state belongs in `Material`; object-specific overrides belong in `MaterialInstance`.
- Pipeline identity depends on material full description strings and render-state flags.
- Use `ensureUniqueMaterial()` before mutating template-level render state from an instance.

## Boundaries

- Shader loading/caching is in `Shader`; GPU descriptor/pipeline creation is in `BackEnd`.
- Textures referenced by material params are `Texture` objects from `Texture`.

## Known Traps

- Descriptor updates read from instance `ShadingParams`; mutating only the template may not affect an existing instance as expected.
- `MaterialPool::getMaterialByName()` returns shared instances; clone or unique-name APIs when per-object mutation is needed.
