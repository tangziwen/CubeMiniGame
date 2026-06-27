# CubeEngine/EngineSrc/Font

## Role

FreeType font loading, glyph metrics, glyph atlas generation, and cached font access.

## Important Objects

- `Font`: FreeType face, font size, glyph lookup, ASCII map generation, glyph metrics, and `GlyphAtlas`.
- `FontMgr`: singleton font cache and default/small/title fonts.
- `GlyphData`: per-character bitmap/metrics data.
- `GlyphAtlas`, `GlyphAtlasNode`: packs glyphs into a texture and maps characters to UV nodes.
