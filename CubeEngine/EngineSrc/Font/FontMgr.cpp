#include "FontMgr.h"

namespace tzw {
TZW_SINGLETON_IMPL(FontMgr)
Font *FontMgr::getFont(std::string fontFile, unsigned int fontSize)
{
    char tmp[50];
    std::string uStr = fontFile;
    uStr.append(itoa(fontSize,tmp,10));
    auto result = m_fontMap.find(uStr);
    if(result != m_fontMap.end())
    {
        return result->second;
    }else
    {
        Font * font = new Font(fontFile,fontSize);
        m_fontMap.insert(std::make_pair(uStr,font));
        return font;
    }
}

void FontMgr::addDefaultFonts()
{
    //init the default font
    m_defaultFont =  getFont("./res/EngineCoreRes/fonts/wenquanyi_zhenghei.ttf",14);
    m_smallFont =  getFont("./res/EngineCoreRes/fonts/wenquanyi_zhenghei.ttf",11);
    m_titleFont = getFont("./res/EngineCoreRes/fonts/wenquanyi_zhenghei.ttf",16);
}

FontMgr::FontMgr()
    :m_defaultFont(nullptr)
{
    addDefaultFonts();
}

Font *FontMgr::getTitleFont() const
{
    return m_titleFont;
}


Font *FontMgr::getSmallFont() const
{
    return m_smallFont;
}

Font *FontMgr::getDefaultFont() const
{
    return m_defaultFont;
}

} // namespace tzw
