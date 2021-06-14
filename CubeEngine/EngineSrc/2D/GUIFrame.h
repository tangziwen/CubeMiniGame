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
	void submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg) override;
	void setContentSize(const vec2 &getContentSize) override;
    bool isInTheRect(vec2 touchPos);
	void setUniformColor(const tzw::vec4 &color) override;
	void setUniformColor(const tzw::vec3 &color) override;
private:
	vec4 m_color;
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_GUIFRAME_H
