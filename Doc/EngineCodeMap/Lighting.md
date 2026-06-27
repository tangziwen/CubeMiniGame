# CubeEngine/EngineSrc/Lighting

## Role

Light data objects used by scenes and deferred lighting.

## Important Objects

- `BaseLight`: color and intensity.
- `DirectionalLight`: sun direction, latitude/longitude/time, and day-elapse helpers.
- `PointLight`: `Drawable3D` plus radius; participates in scene/render queries.
- `SpotLight`: point light plus direction.
- `AmbientLight`: global ambient light value.

## Contracts

- `Scene` owns default directional and ambient lights.
- Point lights are drawables, so scene visibility/culling rules apply.

## Boundaries

- Light shading and GBuffer consumption are in `Rendering`/`BackEnd`; light objects only carry scene data.

## Known Traps

- Directional light angle/time helpers update direction internally; check `tick()`/setter behavior before driving time-of-day externally.
