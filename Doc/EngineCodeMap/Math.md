# CubeEngine/EngineSrc/Math

## Role

Engine math primitives for vectors, matrices, quaternions, bounds, rays, planes, frustums, and rectangles.

## Important Objects

- `vec2`, `vec3`, `vec4`, `ivec2`: basic vector types; `vec3`/`vec4` have optional SIMD paths.
- `Matrix44`: transform/projection matrix, inverse, transpose, decomposition, basis access, perspective/ortho/frustum setup.
- `Quaternion`: Euler/axis/matrix/direction conversion and interpolation.
- `AABB`: bounds update/merge/transform/split/intersection and distance helpers.
- `Ray`: plane/AABB/triangle intersection.
- `Plane`: normal-distance plane, point side, projection.
- `Frustum`: camera frustum planes and AABB culling.
- `Rect`, `t_Sphere`: small geometry helpers.

## Contracts

- Camera/world/render code uses `Matrix44` and `Quaternion`; keep handedness/depth assumptions verified against renderer before changing.
- `AABB` is the shared culling/picking bound type.
- `Ray::intersectTriangle()` returns hit distance through optional out pointer.

## Known Traps

- Some inline math paths do not guard zero-length normalization; use `safeNormalized()` when fallback matters.
- `Matrix44::perspective()` depth convention matters for Vulkan post effects; verify before changing projection math.
