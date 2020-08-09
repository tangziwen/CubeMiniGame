#ifndef TZW_CUBEPRIMITIVE_H
#define TZW_CUBEPRIMITIVE_H
#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
namespace tzw {

class CubePrimitive : public Drawable3D
{
public:
    CubePrimitive(float width, float depth, float height, bool isNeedPreGenerateMat = true);
	void submitDrawCmd(RenderFlag::RenderStage passType) override;
    bool intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint);
	virtual void setColor(vec4 color);
	virtual bool isHit(Ray ray);
	Mesh * getMesh() override;
	Mesh * getMesh(int index) override;
	void setMesh(Mesh * newMesh);
protected:
	vec3 getWorldPos(vec3 localPos);
    void initMesh();
	virtual void checkCollide(ColliderEllipsoid * package);
    Mesh * m_mesh;
    float m_width, m_depth, m_height;
};

} // namespace tzw

#endif // TZW_CUBEPRIMITIVE_H
