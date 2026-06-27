# CubeEngine/EngineSrc/Mesh

## Role

Geometry storage, GPU buffer submission, ray intersection, AABB calculation, and instancing payloads.

## Important Objects

- `Mesh`: vertices/indices/instances, AABB, VBO/IBO/render buffers, merge/subdivide/normal/tangent helpers, ray intersection, and GPU submit/resubmit.
- `InstancedMesh`: instance buffer wrapper for a base `Mesh`.
- `VertexData`: position, normal, UV, color, barycentric, material blend, tangent, overlay color.
- `InstanceData`: per-instance transform/color data.
