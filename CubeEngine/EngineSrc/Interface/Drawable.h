#ifndef TZW_DRAWABLE_H
#define TZW_DRAWABLE_H

#include "../Base/Node.h"
#include "../Base/Camera.h"
#include "../Technique/MaterialInstance.h"
#include "../3D/Effect/Effect.h"
#include "../Rendering/RenderCommand.h"
namespace tzw {
class Drawable : public Node
{
public:
    Drawable();
    Camera *camera() const;
    void setCamera(Camera *camera);
    virtual Node::NodeType getNodeType();
    virtual void reCache();
	virtual MaterialInstance *getMaterial() const;
	virtual void setMaterial(MaterialInstance *technique);
    virtual void setUpTransFormation(TransformationInfo & info);
	virtual void setUpCommand(RenderCommand & command);
	virtual void setColor(vec4 newColor);
	virtual void setColor(vec3 newColorV3);
	virtual vec4 getColor();
	virtual void setOverLayColor(vec4 newColor);
	virtual vec4 getOverLayColor();
	unsigned int getCustomRenderPriority() const;
	void setCustomRenderPriority(unsigned int customRenderPriority);
protected:
    MaterialInstance * m_material;
    Camera * m_camera;
	vec4 m_color;
	vec4 m_overLayColor = {1, 1, 1, 0.0};
	unsigned int m_customRenderPriority = 0;
};

} // namespace tzw

#endif // TZW_DRAWABLE_H
