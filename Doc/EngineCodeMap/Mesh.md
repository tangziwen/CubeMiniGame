# CubeEngine/EngineSrc/Mesh

## Role

Geometry storage, GPU buffer submission, ray intersection, AABB calculation, and instancing payloads.

## Important Objects

- `Mesh`: vertices/indices/instances, AABB, VBO/IBO/render buffers, merge/subdivide/normal/tangent helpers, ray intersection, and GPU submit/resubmit.
- `InstancedMesh`: instance buffer wrapper for a base `Mesh`.
- `VertexData`: position, normal, UV, color, barycentric, material blend, tangent, overlay color.
- `InstanceData`: per-instance transform/color data.

## Contracts

- Call `finish()` or explicit submit methods after building geometry that must render.
- Call `calculateAABB()` / keep AABB current for culling and picking.
- Use `submitInstanced()` / `reSubmitInstanced()` when changing instance lists.

## Boundaries

- Mesh does not own scene lifetime or material routing.
- Backend buffer details are abstracted through `RenderBuffer`.

## Known Traps

- `Mesh::m_indices` stores `short_u`; very large meshes need index-size awareness.
