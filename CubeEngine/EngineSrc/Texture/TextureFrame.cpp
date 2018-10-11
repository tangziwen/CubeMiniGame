#include "TextureFrame.h"
#include "TextureAtlas.h"
namespace tzw {

TextureFrame::TextureFrame(): x(0), y(0), w(0), h(0), m_sheet(nullptr)
{
}

TextureAtlas *TextureFrame::sheet() const
{
    return m_sheet;
}

void TextureFrame::setSheet(TextureAtlas *sheet)
{
    m_sheet = sheet;
}

Texture *TextureFrame::getTexture()
{
    return m_sheet->texture();
}

std::string TextureFrame::getName() const
{
    return m_name;
}

void TextureFrame::setName(const std::string &name)
{
    m_name = name;
}

float TextureFrame::U(float fakeU)
{
    int pos = x + w*fakeU;
    return 1.0f*pos / m_sheet->width();
}

float TextureFrame::V(float fakeV)
{
    fakeV = 1.0 - fakeV;
    auto theY = m_sheet->height() - y;
    int pos = theY - h*fakeV;
    return 1.0f*pos / m_sheet->height();
}

vec2 TextureFrame::UV(float u, float v)
{
    return vec2(U(u),V(v));
}

} // namespace tzw
