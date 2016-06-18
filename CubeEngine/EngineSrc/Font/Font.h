#ifndef TZW_FONT_H
#define TZW_FONT_H
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "../Texture/Texture.h"
#include <map>
#include "../Math/Rect.h"
#include "GlyphData.h"
#include "GlyphAtlas.h"
namespace tzw {


class Texture;

class Font
{
public:
    Font(std::string fontFilePath, unsigned int fontSize);
    FT_Face *face() const;
    void setFace(FT_Face *face);
    unsigned int getCharIndex(unsigned long c);
    GlyphData getGlyphDataFromChar(unsigned long c);
    void generateAsciiMap();
    unsigned int getFontSize() const;
    bool isUnvisibleChar(unsigned long c);
    unsigned int getSpecialAdvance(unsigned long c, unsigned int baseAdvance);
    GlyphAtlas *getGlyphAtlas() const;
    void setGlyphAtlas(GlyphAtlas *glyphAtlas);
private:
    void initAsciiData();
    unsigned int m_fontSize;
    FT_Face * m_face;
    GlyphAtlas * m_glyphAtlas;
};

} // namespace tzw

#endif // TZW_FONT_H
