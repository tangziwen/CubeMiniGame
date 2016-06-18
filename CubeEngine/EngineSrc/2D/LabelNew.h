#ifndef TZW_LABELNEW_H
#define TZW_LABELNEW_H
#include "../Font/Font.h"
#include "../Mesh/Mesh.h"
#include "../Interface/Drawable.h"

namespace tzw {

class LabelNew : public Drawable
{
public:
    LabelNew();
    static LabelNew * create(std::string theString);
    static LabelNew * create(std::string theString, Font * getFont);
    static LabelNew * create(Font * getFont);
    std::string getString() const;
    void setString(const std::string &getString);
    Font *getFont() const;
    void setFont(Font *font);
    void genMesh();
    virtual void draw();
    vec2 contentSize() const;
    void setContentSize(const vec2 &contentSize);
    void initAtlas();
private:
    GlyphAtlas * m_atlas;
    std::string m_string;
    Font * m_font;
    Mesh * m_mesh;
    vec2 m_contentSize;
};

} // namespace tzw

#endif // TZW_LABELNEW_H
