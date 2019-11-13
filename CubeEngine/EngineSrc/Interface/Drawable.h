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
    Material *getMaterial() const;
	virtual void setMaterial(Material *technique);
    virtual void setUpTransFormation(TransformationInfo & info);
	virtual void setUpCommand(RenderCommand & command);
	virtual void setColor(vec4 newColor);
	virtual vec4 getColor();
protected:
    Material * m_material;
    Camera * m_camera;
	vec4 m_color;
};

} // namespace tzw

#endif // TZW_DRAWABLE_H
