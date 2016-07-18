#ifndef TZW_GUIFRAME_H
#define TZW_GUIFRAME_H
#include "../Mesh/Mesh.h"
#include "../Interface/Drawable.h"

namespace tzw {

class GUIFrame: public Drawable
{
public:
    GUIFrame();
    void setRenderRect();
    static GUIFrame * create(vec4 color,vec2 size);
    static GUIFrame * create(vec4 color);
    static GUIFrame * create(vec2 size);
    virtual void draw();
    vec2 getContentSize() const;
    virtual void setContentSize(const vec2 &contentSize);
    bool isInTheRect(vec2 touchPos);
protected:
    vec2 m_contentSize;
private:
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_GUIFRAME_H
