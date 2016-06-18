#ifndef TZW_LABEL_H
#define TZW_LABEL_H

#include <string>
#include <vector>
#include "../Base/Node.h"
#include "../Interface/Drawable.h"


#include "Glyph.h"
namespace tzw {

class Label: public Drawable
{
public:
    Label();
    static Label * create(std::string text,Font * font);
    static Label * create(std::string text,std::string fontFilePath, unsigned int fontSize = 11);
    void initWithFont(std::string fontFilePath, unsigned int fontSize);
    void initWithFont(Font * font);
    std::string str() const;
    void setStr(const std::string &str);
    Font *font() const;
    void setFont(Font *font);
    vec2 contentSize() const;
    void setContentSize(const vec2 &contentSize);
    virtual void setUniformColor(const vec4 &uniformColor);
    virtual void setUniformColor(const vec3 &color);

private:
    std::string m_str;
    std::vector<Glyph *>m_glyphList;
    Font * m_font;
    vec2 m_contentSize;
};

} // namespace tzw

#endif // TZW_LABEL_H
