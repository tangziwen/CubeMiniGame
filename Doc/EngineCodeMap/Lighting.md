# CubeEngine/EngineSrc/Lighting

## Role

Light data objects used by scenes and deferred lighting.

## Important Objects

- `BaseLight`: color and intensity.
- `DirectionalLight`: sun direction, latitude/longitude/time, and day-elapse helpers.
- `PointLight`: `Drawable3D` plus radius; participates in scene/render queries.
- `SpotLight`: point light plus direction.
- `AmbientLight`: global ambient light value.
