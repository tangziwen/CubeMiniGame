#ifndef TZW_GLYPHDATA_H
#define TZW_GLYPHDATA_H
#include <ft2build.h>
#include FT_FREETYPE_H



namespace tzw {

class Font;
///表征的是一个字符串的数据信息，该对象是易变的
///每次调用都会使原有数据失效
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
