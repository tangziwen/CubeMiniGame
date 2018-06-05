#ifndef TZW_GLYPHDATA_H
#define TZW_GLYPHDATA_H
#include <ft2build.h>
#include FT_FREETYPE_H



namespace tzw {

class Font;
struct GlyphData{
    unsigned int rows;
    unsigned int width;
    unsigned int height_pow;
    unsigned int width_pow;
    unsigned char * buffer;
    unsigned char * glBuffer;
    int top;
    int left;
    int advance;
    void initFromFace(FT_Face *face, unsigned long c);
    unsigned int getSpecialAdvance(unsigned long c,unsigned int baseAdvance);
    bool isUnvisibleChar(unsigned long c);

    unsigned long m_char;
};

} // namespace tzw

#endif // TZW_GLYPHDATA_H
