#ifndef TZW_LABELNEW_H
#define TZW_LABELNEW_H
#include "../Font/Font.h"
#include "../Mesh/Mesh.h"
#include "../Interface/Drawable2D.h"

namespace tzw {

class LabelNew : public Drawable2D
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
    void initAtlas();
private:
    GlyphAtlas * m_atlas;
    std::string m_string;
    Font * m_font;
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_LABELNEW_H
