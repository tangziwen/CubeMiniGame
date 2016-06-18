#ifndef TZW_GLYPH_H
#define TZW_GLYPH_H
#include "Sprite.h"
#include "../Font/Font.h"
namespace tzw {

class Glyph : public Sprite
{
public:
    Glyph();
    static Glyph * create(unsigned long c, Font * font);
    Font *font() const;
    void setFont(Font *font);

    GlypRenderInfo *info() const;
    void setInfo(GlypRenderInfo *info);

private:
    GlypRenderInfo * m_info;
    Font * m_font;
};

} // namespace tzw

#endif // TZW_GLYPH_H
