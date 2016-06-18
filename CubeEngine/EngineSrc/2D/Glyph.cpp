#include "Glyph.h"
namespace tzw {

Glyph::Glyph()
{

}

Glyph *Glyph::create(unsigned long c, Font *font)
{
    auto g = new Glyph();
    auto info = font->getGlyph(c);
    g->m_info = info;
    g->initWithTexture(info->texture);
    auto texSize = g->m_texture->getSize();
    float x=(float)g->info()->outWidth / (float)texSize.getX(),
    y=(float)g->info()->outHeight / (float)texSize.getY();
    g->setRenderRect(vec4(0,g->info()->outHeight,0,0),vec4(0,0,0,y),vec4(g->info()->outWidth,0,x,y),vec4(g->info()->outWidth,g->info()->outHeight,x,0));
    g->m_font = font;
    return g;
}


Font *Glyph::font() const
{
    return m_font;
}

void Glyph::setFont(Font *font)
{
    m_font = font;
}
GlypRenderInfo *Glyph::info() const
{
    return m_info;
}

void Glyph::setInfo(GlypRenderInfo *info)
{
    m_info = info;
}



} // namespace tzw

