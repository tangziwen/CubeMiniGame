#ifndef TZW_CYLINDER_PRIMITVE_H
#define TZW_CYLINDER_PRIMITVE_H
#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
namespace tzw {

class CylinderPrimitive : public Drawable3D
{
public:
    CylinderPrimitive(float radiusTop, float radiusBottom, float height);
    virtual void submitDrawCmd(RenderCommand::RenderType passType);
    bool intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint);
	virtual void setColor(vec4 color);
protected:
	vec3 getWorldPos(vec3 localPos);
    void initMesh();
	virtual void checkCollide(ColliderEllipsoid * package);
    Mesh * m_mesh;
    float m_radiusTop, m_radiusBottom, m_height;
	vec4 m_color;
	vec3 getSegPos(float theta, int side);
};

} // namespace tzw

#endif // TZW_CYLINDER_PRIMITVE_H