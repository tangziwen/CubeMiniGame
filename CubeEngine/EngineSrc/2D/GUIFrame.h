#ifndef TZW_GUIFRAME_H
#define TZW_GUIFRAME_H
#include "../Mesh/Mesh.h"
#include "../Interface/Drawable2D.h"

namespace tzw {

class GUIFrame: public Drawable2D
{
public:
    GUIFrame();
    void setRenderRect();
    static GUIFrame * create(vec4 color,vec2 size);
    static GUIFrame * create(vec4 color);
    static GUIFrame * create(vec2 size);
    virtual void submitDrawCmd();
    virtual void setContentSize(const vec2 &getContentSize);
    bool isInTheRect(vec2 touchPos);
protected:
private:
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_GUIFRAME_H
