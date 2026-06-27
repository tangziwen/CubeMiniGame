# CubeEngine/EngineSrc/Texture

## Role

CPU-side texture objects, texture caching, atlases, and texture frames.

## Important Objects

- `Texture`: file/raw/cubemap texture wrapper. Owns file path, dimensions, filter/wrap settings, mip flag, texture type, and backend `DeviceTexture`.
- `TextureMgr`: singleton cache for 2D textures, async load, cubemaps, and single-cubemap files.
- `TextureAtlas`: sheet texture plus named `TextureFrame` lookup.
- `TextureFrame`: UV/rect frame data inside an atlas.
