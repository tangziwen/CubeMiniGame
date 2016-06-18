#include "Label.h"
#include "../Font/FontMgr.h"
namespace tzw {

Label::Label()
    :m_font(nullptr)
{

}

Label *Label::create(std::string text, Font *font)
{
    auto label = new Label();
    label->initWithFont(font);
    label->setStr(text);
    return label;
}

Label *Label::create(std::string text, std::string fontFilePath, unsigned int fontSize)
{
    auto label = new Label();
    label->initWithFont(fontFilePath,fontSize);
    label->setStr(text);
    return label;
}

void Label::initWithFont(std::string fontFilePath, unsigned int fontSize)
{
    m_font = FontMgr::shared()->getFont(fontFilePath,fontSize);
}

void Label::initWithFont(Font *font)
{
    m_font = font;
}

std::string Label::str() const
{
    return m_str;
}

void Label::setStr(const std::string &str)
{
    if(!m_glyphList.empty())//remove preivous
    {
        for(auto iter = m_glyphList.begin();iter!=m_glyphList.end();iter++)
        {
            Glyph * myGlyph = (*iter);
            myGlyph->removeFromParent();
        }
        m_glyphList.clear();
    }
    m_str = str;
    int pen_x = 0,pen_y =0;
    for(int i =0;i<str.size();i++)
    {
        auto c = str[i];
        Glyph * g;
        g = Glyph::create(c,m_font);
        if(!m_font->isUnvisibleChar(c))
        {
            int diff = - int(g->info()->outHeight) + int(g->info()->top);
            g->setPos2D(pen_x,pen_y + diff);
            g->setZorder(2);
            addChild(g);
        }
        pen_x += g->info()->x_adavance;
        m_glyphList.push_back(g);
        if(c =='\n')
        {
            pen_y  -= m_font->getFontSize();
            pen_x = 0;
        }
    }
    m_contentSize = vec2(pen_x,fabs(pen_y) + m_font->getFontSize());
}

Font *Label::font() const
{
    return m_font;
}

void Label::setFont(Font *font)
{
    m_font = font;
}

vec2 Label::contentSize() const
{
    return m_contentSize;
}

void Label::setContentSize(const vec2 &contentSize)
{
    m_contentSize = contentSize;
}

void Label::setUniformColor(const vec4 &uniformColor)
{
    Drawable::setUniformColor(uniformColor);
    for(int i =0;i<m_glyphList.size();i++)
    {
        Glyph * g = m_glyphList[i];
        g->setUniformColor(uniformColor);
    }
}

void Label::setUniformColor(const vec3 &color)
{
    setUniformColor(vec4(color.x,color.y,color.z,1.0));
}


} // namespace tzw

