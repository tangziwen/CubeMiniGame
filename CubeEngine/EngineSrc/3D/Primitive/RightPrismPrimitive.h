#pragma once

#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
namespace tzw {

class RightPrismPrimitive : public Drawable3D
{
public:
    RightPrismPrimitive(float width, float height, float depth, bool isNeedPreGenerateMat = true);
	void submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueues * queues, int requirementArg) override;
    bool intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint);
	virtual void setColor(vec4 color);
	virtual bool isHit(Ray ray);
	Mesh * getMesh() override;
	Mesh * getMesh(int index) override;
protected:
	vec3 getWorldPos(vec3 localPos);
    void initMesh();
	virtual void checkCollide(ColliderEllipsoid * package);
    Mesh * m_mesh;
    float m_width;
	float m_depth;
	float m_height;
};

} // namespace tzw
