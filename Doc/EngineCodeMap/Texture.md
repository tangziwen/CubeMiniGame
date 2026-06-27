# CubeEngine/EngineSrc/Texture

## Role

CPU-side texture objects, texture caching, atlases, and texture frames.

## Important Objects

- `Texture`: file/raw/cubemap texture wrapper. Owns file path, dimensions, filter/wrap settings, mip flag, texture type, and backend `DeviceTexture`.
- `TextureMgr`: singleton cache for 2D textures, async load, cubemaps, and single-cubemap files.
- `TextureAtlas`: sheet texture plus named `TextureFrame` lookup.
- `TextureFrame`: UV/rect frame data inside an atlas.

## Contracts

- Use `TextureMgr` for shared file textures.
- `Texture::handle()` / `getTextureId()` expose backend texture resource for descriptor binding.
- Call `genMipMap()` or request mip path when materials depend on mip filtering.

## Boundaries

- GPU image allocation/upload is backend-owned.
- Font glyph atlases are built in `Font` but produce `Texture`.

## Known Traps

- Raw-data constructor has `needFlipY`; verify coordinate expectations before reusing for generated textures.
