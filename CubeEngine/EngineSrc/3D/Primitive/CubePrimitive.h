#ifndef TZW_CUBEPRIMITIVE_H
#define TZW_CUBEPRIMITIVE_H
#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
namespace tzw {

class CubePrimitive : public Drawable3D
{
public:
    CubePrimitive(float width, float depth, float height, bool isNeedPreGenerateMat = true);
    virtual void submitDrawCmd(RenderCommand::RenderType passType);
    bool intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint);
	virtual void setColor(vec4 color);
protected:
	vec3 getWorldPos(vec3 localPos);
    void initMesh();
	virtual void checkCollide(ColliderEllipsoid * package);
    Mesh * m_mesh;
    float m_width, m_depth, m_height;
	vec4 m_color;

};

} // namespace tzw

#endif // TZW_CUBEPRIMITIVE_H
