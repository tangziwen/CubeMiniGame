#include "GlyphData.h"
#include "External/TUtility/TUtility.h"
namespace tzw {

void GlyphData::initFromFace(FT_Face *face,unsigned long c)
{
    m_char = c;
    rows = (*face)->glyph->bitmap.rows;
    width = (*face)->glyph->bitmap.width;
    buffer = static_cast<unsigned char *>(malloc(rows * width * sizeof(char)));
    memcpy(buffer,(*face)->glyph->bitmap.buffer,rows * width);
    if(!isUnvisibleChar(c))
    {
        advance = (static_cast<int>((*face)->glyph->advance.x >> 6));
    }else
    {
        advance = getSpecialAdvance(c,(static_cast<int>((*face)->glyph->advance.x >> 6)));
    }

    top = (*face)->glyph->bitmap_top;
    left = (*face)->glyph->bitmap_left;

    height_pow = TbaseMath::nextPow2(rows);
    width_pow = TbaseMath::nextPow2(width);
    //expand the data to opengl compactable:
    glBuffer = new unsigned char[ 2 * width_pow * height_pow];
    for(int j=0; j <height_pow;j++) {
        for(int i=0; i < width_pow; i++){
            glBuffer[2*(i+j*width_pow)]= glBuffer[2*(i+j*width_pow)+1] =
                    (i>=width || j>=rows) ?
                        0 : buffer[i + width*j];
        }
    }
}

unsigned int GlyphData::getSpecialAdvance(unsigned long c,unsigned int baseAdvance)
{
    switch(c)
    {
    case '\n':
        return 0;
        break;
    case '\t':
        return 4*baseAdvance;
        break;
    default:
        return 0;
    }
}

bool GlyphData::isUnvisibleChar(unsigned long c)
{
    if(c<32)
    {
        return true;
    }
    else
    {
        return false;
    }
}
} // namespace tzw
