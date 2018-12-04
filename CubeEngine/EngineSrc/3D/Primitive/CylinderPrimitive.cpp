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
	float step = 2 * 3.14 / seg;
	float theta = 0.0;
	for(int i = 0; i < seg; i++)
	{
		//middle 
		vec3 down_1 = getSegPos(theta, 0);
		vec3 down_2 = getSegPos(theta + step, 0);
		vec3 up_1 = getSegPos(theta, 1);
		vec3 up_2 = getSegPos(theta + step, 1);
	
		m_mesh->addVertex(VertexData(up_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(down_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(down_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addIndex(i);
		m_mesh->addIndex(i + 1);
		m_mesh->addIndex(i + 2);

		m_mesh->addVertex(VertexData(down_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(up_1, vec2(0.0f, 0.0f), m_color));
		m_mesh->addVertex(VertexData(up_2, vec2(0.0f, 0.0f), m_color));
		m_mesh->addIndex(i + 3);
		m_mesh->addIndex(i + 4);
		m_mesh->addIndex(i + 5);


		theta += step;

	}
    m_mesh->caclNormals();
    m_mesh->finish();
    m_localAABB.merge(m_mesh->getAabb());
    reCache();
    reCacheAABB();
}

void CylinderPrimitive::checkCollide(ColliderEllipsoid * package)
{
	if(!m_isHitable) return;
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
vec3 CylinderPrimitive::getSegPos(float theta, int side)
{
	float x = cos(theta);
	float y = sin(theta);
	float halfHeight = m_height / 2.0;
	if (side)
	{
		return vec3(x, y, halfHeight);
	}
	else
	{
		return vec3(x, y, -halfHeight);
	}
}
} // namespace tzw
