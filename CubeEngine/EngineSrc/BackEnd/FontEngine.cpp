#include "FontEngine.h"
#include "../Font/Font.h"
namespace tzw {
FontEngine * FontEngine::m_instance = nullptr;
FontEngine *FontEngine::shared()
{
    if(!m_instance)
    {
        m_instance = new FontEngine();
    }
    return m_instance;
}

void FontEngine::initFont(Font *font, std::string fontFilePath,unsigned int fontSize)
{
    auto face = font->face();
    FT_New_Face( *m_library,fontFilePath.c_str(),0,face);

    FT_Set_Char_Size(*face,fontSize<<6,0,96,96);
}

FontEngine::FontEngine()
{
    m_library = new FT_Library();
    FT_Init_FreeType(m_library);
}

} // namespace tzw

