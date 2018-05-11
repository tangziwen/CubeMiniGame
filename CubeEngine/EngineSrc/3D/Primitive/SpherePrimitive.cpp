#include "SpherePrimitive.h"
#include "../../Rendering/RenderCommand.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "../EngineSrc/Collision/CollisionUtility.h"
namespace tzw
{



	SpherePrimitive::SpherePrimitive(float radius, int resolution)
	{
		m_radius = radius;
		m_resolution = resolution;
		m_material = Material::createFromTemplate("Color");
		initMesh();
		setCamera(g_GetCurrScene()->defaultCamera());
		setIsAccpectOCTtree(false);
		initMesh();
	}

	void SpherePrimitive::submitDrawCmd(RenderCommand::RenderType passType)
	{
		RenderCommand command(m_mesh, m_material,RenderCommand::RenderType::Common);
		setUpTransFormation(command.m_transInfo);
		Renderer::shared()->addRenderCommand(command);
	}

	bool SpherePrimitive::intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint)
	{
		return false;
	}

	tzw::vec3 SpherePrimitive::pointOnSurface(float u, float v)
	{
		return vec3(cos(u) * sin(v) * m_radius, cos(v) * m_radius, sin(u) * sin(v) * m_radius);
	}

	tzw::vec3 SpherePrimitive::getWorldPos(vec3 localPos)
	{
		auto theMat = getTransform();
		return theMat.transformVec3(localPos);
	}

	void SpherePrimitive::initMesh()
	{
		m_mesh = new Mesh();
		float PI = 3.1416;
		float startU=0;
		float startV=0;
		float endU=PI*2;
		float endV=PI;
		float stepU=(endU-startU)/m_resolution; // step size between U-points on the grid
		float stepV=(endV-startV)/m_resolution; // step size between V-points on the grid
		for(int i=0;i<m_resolution;i++){ // U-points
			for(int j=0;j<m_resolution;j++){ // V-points
				float u=i*stepU+startU;
					float v=j*stepV+startV;
					float un=(i+1==m_resolution) ? endU : (i+1)*stepU+startU;
					float vn=(j+1==m_resolution) ? endV : (j+1)*stepV+startV;
					// Find the four points of the grid
					// square by evaluating the parametric
					// surface function
					vec3 p0=pointOnSurface(u, v);
					vec3 p1=pointOnSurface(u, vn);
					vec3 p2=pointOnSurface(un, v);
					vec3 p3=pointOnSurface(un, vn);
					// NOTE: For spheres, the normal is just the normalized
					// version of each vertex point; this generally won't be the case for
					// other parametric surfaces.
					// Output the first triangle of this grid square
						
					m_mesh->addVertex(VertexData(p0, p0.normalized()));
					m_mesh->addVertex(VertexData(p2, p2.normalized()));
					m_mesh->addVertex(VertexData(p1, p1.normalized()));
					m_mesh->addIndex(m_mesh->getIndicesSize());
					m_mesh->addIndex(m_mesh->getIndicesSize());
					m_mesh->addIndex(m_mesh->getIndicesSize());


					m_mesh->addVertex(VertexData(p3, p3.normalized()));
					m_mesh->addVertex(VertexData(p1, p1.normalized()));
					m_mesh->addVertex(VertexData(p2, p2.normalized()));
					m_mesh->addIndex(m_mesh->getIndicesSize());
					m_mesh->addIndex(m_mesh->getIndicesSize());
					m_mesh->addIndex(m_mesh->getIndicesSize());
			}
		}
		m_mesh->finish();
		m_localAABB.merge(m_mesh->getAabb());
		reCache();
		reCacheAABB();
	}

	void SpherePrimitive::checkCollide(ColliderEllipsoid * package)
	{
		auto size = m_mesh->getIndicesSize();
		std::vector<vec3> resultList;
		float t = 0;
		for (auto i =0; i< size; i+=3)
		{
			CollisionUtility::checkTriangle(package,
				package->toE(getWorldPos (m_mesh->m_vertices[m_mesh->getIndex(i)].m_pos)),
				package->toE(getWorldPos (m_mesh->m_vertices[m_mesh->getIndex(i + 1)].m_pos)),
				package->toE(getWorldPos (m_mesh->m_vertices[m_mesh->getIndex(i + 2)].m_pos)));
		}
	}

}