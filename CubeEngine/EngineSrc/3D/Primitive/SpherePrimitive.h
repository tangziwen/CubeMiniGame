#ifndef TZW_SPHERE_PRIMITIVE_H
#define TZW_SPHERE_PRIMITIVE_H


#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"

namespace tzw
{
	class SpherePrimitive  : public Drawable3D
	{
	public:
		SpherePrimitive(float radius, int resolution);
		void submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg) override;
		bool intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint);
		vec3 pointOnSurface(float u, float v);
		vec3 getWorldPos(vec3 localPos);
		float radius() const;
		void setRadius(float radius);
		int resolution() const;
		void setResolution(int resolution);
		Mesh * getMesh() override;
		Mesh * getMesh(int index) override;
	protected:
		void initMesh();
		virtual void checkCollide(ColliderEllipsoid * package);
		Mesh * m_mesh;
		float m_radius;
		int m_resolution;
	};

}



#endif