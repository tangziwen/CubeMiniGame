#ifndef TZW_DRAWABLE_H
#define TZW_DRAWABLE_H

#include "../Base/Node.h"
#include "../Base/Camera.h"
#include "../Technique/Material.h"
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
	virtual Material *getMaterial() const;
	virtual void setMaterial(Material *technique);
    virtual void setUpTransFormation(TransformationInfo & info);
	virtual void setUpCommand(RenderCommand & command);
	virtual void setColor(vec4 newColor);
    virtual void setColor(vec3 newColorV3);
	virtual vec4 getColor();
	virtual void setOverLayColor(vec4 newColor);
	virtual vec4 getOverLayColor();
protected:
    Material * m_material;
    Camera * m_camera;
	vec4 m_color;
	vec4 m_overLayColor = {1, 1, 1, 0.0};
};

} // namespace tzw

#endif // TZW_DRAWABLE_H
