#include "Font.h"
#include "../BackEnd/FontEngine.h"
#include "External/TUtility/TUtility.h"

namespace tzw {

Font::Font(std::string fontFilePath,unsigned int fontSize)
{
    m_face = new FT_Face();
    FontEngine::shared()->initFont(this,fontFilePath,fontSize);
    m_fontSize = fontSize;
    initAsciiData();
}

FT_Face *Font::face() const
{
    return m_face;
}

void Font::setFace(FT_Face *face)
{
    m_face = face;
}

unsigned int Font::getCharIndex(unsigned long c)
{
    return FT_Get_Char_Index( *m_face,c);
}

GlyphData Font::getGlyphDataFromChar(unsigned long c)
{
    FT_Load_Char(*m_face,c,FT_LOAD_DEFAULT|FT_LOAD_RENDER);
    GlyphData g;
    g.initFromFace(m_face,c);
    return g;
}

void Font::generateAsciiMap()
{
    GlyphAtlas atlas;
    for(int i =0;i<128;i++)
    {
        atlas.addGlyphData(getGlyphDataFromChar(i));
    }
    atlas.generate();
    atlas.test();
}

unsigned int Font::getFontSize() const
{
    return m_fontSize;
}

bool Font::isUnvisibleChar(unsigned long c)
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


void Font::initAsciiData()
{
    m_glyphAtlas = new GlyphAtlas();
    for(int i =0;i<128;i++)
    {
        m_glyphAtlas->addGlyphData(getGlyphDataFromChar(i));
    }
    m_glyphAtlas->generate();
    m_glyphAtlas->generateGLTexture();
}

GlyphAtlas *Font::getGlyphAtlas() const
{
    return m_glyphAtlas;
}

void Font::setGlyphAtlas(GlyphAtlas *glyphAtlas)
{
    m_glyphAtlas = glyphAtlas;
}

} // namespace tzw

