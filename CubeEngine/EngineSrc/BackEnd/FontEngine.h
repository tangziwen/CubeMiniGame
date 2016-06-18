#ifndef TZW_FONTENGINE_H
#define TZW_FONTENGINE_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
namespace tzw {

class Font;
class FontEngine
{
public:
    static FontEngine *shared();
    void initFont(Font* font, std::string fontFilePath, unsigned int fontSize);
private:
    FontEngine();
    static FontEngine * m_instance;
    FT_Library * m_library;
};

} // namespace tzw

#endif // TZW_FONTENGINE_H
