#include "CylinderPrimitive.h"
#include <algorithm>
#include "../../Rendering/RenderCommand.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "../EngineSrc/Collision/CollisionUtility.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Collision/PhysicsMgr.h"
namespace tzw {

CylinderPrimitive::CylinderPrimitive(float radiusTop, float radiusBottom, float height)
{
    m_radiusTop = radiusTop;
    m_radiusBottom = radiusBottom;
    m_height = height;
	m_mesh = nullptr;
	m_color = vec4::fromRGB(255,255,255);
    m_material = Material::createFromTemplate("ModelStd");
	auto texture =  TextureMgr::shared()->getByPath("Texture/rock.jpg");
	m_material->setTex("diffuseMap", texture);
    initMesh();
    setCamera(g_GetCurrScene()->defaultCamera());
    setIsAccpectOcTtree(true);
}

void CylinderPrimitive::submitDrawCmd(RenderCommand::RenderType passType)
{
	if(getIsVisible())
	{
		RenderCommand command(m_mesh, m_material,passType);
		setUpCommand(command);
		Renderer::shared()->addRenderCommand(command);
	}
}

bool CylinderPrimitive::intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint)
{
    auto theMat = getTransform();

    auto size = m_mesh->getIndicesSize();
    std::vector<vec3> resultList;
    float t = 0;
    for (auto i =0; i< size; i+=3)
    {
        vec3 tmpHitPoint;
        if(sphere.intersectWithTriangle(theMat.transformVec3(m_mesh->m_vertices[i + 2].m_pos),
                                        theMat.transformVec3(m_mesh->m_vertices[i + 1].m_pos), theMat.transformVec3(m_mesh->m_vertices[i].m_pos), tmpHitPoint))
        {
            resultList.push_back(tmpHitPoint);
        }
    }
    if(!resultList.empty())
    {
        std::sort(resultList.begin(),resultList.end(),[sphere](const vec3 & v1, const vec3 & v2)    {
            float dist1 = sphere.centre().distance(v1);
            float dist2 = sphere.centre().distance(v2);
            return dist1<dist2;
        });
        hitPoint = resultList;
        return true;
    }
    return false;
}

void CylinderPrimitive::setColor(vec4 color)
{
	m_color = color;
	initMesh();
}

vec3 CylinderPrimitive::getWorldPos(vec3 localPos)
{
	auto theMat = getTransform();
	return theMat.transformVec3(localPos);
}

void CylinderPrimitive::initMesh()
{

	if (!m_mesh)
	{
		m_mesh = new Mesh();
	}
	else
	{
		m_mesh->clear();
	}
	int seg = 20;
	float step = 2 * 3.141592654 / seg;
	float theta = 0.0;
	int index = 0;
	for(int i = 0; i < seg; i++)
	{
		
		vec3 down_1 = getSegPos(theta, 0);
		vec3 down_2 = getSegPos(theta + step, 0);
		vec3 up_1 = getSegPos(theta, 1);
		vec3 up_2 = getSegPos(theta + step, 1);
		//middle 
		m_mesh->addVertex(VertexData(up_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(down_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(down_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addIndex(index);
		m_mesh->addIndex(index + 1);
		m_mesh->addIndex(index + 2);

		m_mesh->addVertex(VertexData(down_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(up_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(up_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addIndex(index + 3);
		m_mesh->addIndex(index + 4);
		m_mesh->addIndex(index + 5);

		float halfHeight = m_height / 2.0;
		//top
		vec3 centerTop = vec3(0.0, 0.0, halfHeight);
		m_mesh->addVertex(VertexData(up_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(up_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(centerTop, vec2(0.0f, 0.0f), m_color));
		m_mesh->addIndex(index + 6);
		m_mesh->addIndex(index + 7);
		m_mesh->addIndex(index + 8);

		//bottom
		vec3 centerBottom = vec3(0.0, 0.0, -halfHeight);
		m_mesh->addVertex(VertexData(down_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(down_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(centerBottom, vec2(0.0f, 0.0f), m_color));
		m_mesh->addIndex(index + 9);
		m_mesh->addIndex(index + 10);
		m_mesh->addIndex(index + 11);
		theta += step;
		index += 12;

	}
    m_mesh->caclNormals();
    m_mesh->finish();
    m_localAABB.merge(m_mesh->getAabb());
    reCache();
    reCacheAABB();
}

void CylinderPrimitive::checkCollide(ColliderEllipsoid * package)
{
	return;
}
vec3 CylinderPrimitive::getSegPos(float theta, int side)
{
	float radius = 0.0;
	float halfHeight = 0.0;
	if (side)
	{
		radius = m_radiusTop;
		halfHeight = m_height / 2.0;
	}
	else
	{
		halfHeight = -m_height / 2.0;
		radius = m_radiusBottom;
	}
	float x = cos(theta) * radius;
	float y = sin(theta) * radius;
	return vec3(x, y, halfHeight);
}
} // namespace tzw
