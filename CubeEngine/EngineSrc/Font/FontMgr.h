#ifndef TZW_FONTMGR_H
#define TZW_FONTMGR_H
#include "Font.h"
#include <map>
#include "../Engine/EngineDef.h"
namespace tzw {

class FontMgr
{
public:
    TZW_SINGLETON_DECL(FontMgr)
    Font *getFont(std::string fontFile, unsigned int fontSize);
    Font *getDefaultFont() const;

    Font *getSmallFont() const;

    Font *getTitleFont() const;
private:
    void addDefaultFonts();
    std::map<std::string,Font *> m_fontMap;
    FontMgr();
    Font * m_defaultFont;
    Font * m_smallFont;
    Font * m_titleFont;
};

} // namespace tzw

#endif // TZW_FONTMGR_H
