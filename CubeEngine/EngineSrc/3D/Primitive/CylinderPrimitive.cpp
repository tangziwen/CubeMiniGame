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
	auto texture =  TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultBaseColor.png");
	m_material->setTex("DiffuseMap", texture);

	m_topBottomMaterial = Material::createFromTemplate("ModelStd");
	auto arrowTexture = TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultBaseColor.png");
	m_topBottomMaterial->setTex("DiffuseMap", arrowTexture);

    initMesh();
    setCamera(g_GetCurrScene()->defaultCamera());
    setIsAccpectOcTtree(true);
}

void CylinderPrimitive::submitDrawCmd(RenderCommand::RenderType passType)
{
	if(getIsVisible())
	{
		RenderCommand command(m_mesh, m_material, passType);
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

vec3 CylinderPrimitive::XYZ2RhoPhiZ(vec3 xyz)
{
	vec3 result;
	result.z = (xyz.z / (this->m_height * 0.5)) * 0.5 + 0.5;

	//Rho
	result.x = sqrt(xyz.x * xyz.x + xyz.y * xyz.y);

	//phi
	result.y = atan2f(xyz.y, xyz.x);
	return result;
}

void CylinderPrimitive::setTex(Texture * texture)
{
	m_material->setTex("DiffuseMap", texture);
}


void CylinderPrimitive::setTopBottomTex(Texture * texture)
{
	m_topBottomMaterial->setTex("DiffuseMap", texture);
}

bool CylinderPrimitive::isHit(Ray ray)
{
	auto mat = getTransform().inverted();
	auto localPos = mat.transformVec3(ray.origin());
	auto localDir = mat.transofrmVec4(vec4(ray.direction(), 0.0));
	return m_mesh->intersectWithRay(Ray(localPos, localDir.toVec3()),nullptr);
}

Mesh* CylinderPrimitive::getMesh()
{
	return m_mesh;
}

Mesh* CylinderPrimitive::getMesh(int index)
{
	return m_mesh;
}

vec2 circleUV(vec3 point, vec3 centre, float radius, bool flipped = false)
{
	return vec2((point.x - centre.x) / (2.0 * radius) + 0.5, (point.y - centre.y) / (2.0 * radius) + 0.5);
	if(!flipped) 
	{
		return vec2((point.x - centre.x) / (2.0 * radius) + 0.5, (point.y - centre.y) / (2.0 * radius) + 0.5);
	} 
	else 
	{
		return vec2((point.x - centre.x) / (2.0 * radius) + 0.5, 1.0 - ((point.y - centre.y) / (2.0 * radius) + 0.5));
	}
	

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
		m_mesh->addVertex(VertexData(up_1, vec2(theta / (2 * 3.14), 1.0), m_color));
		m_mesh->addVertex(VertexData(down_2, vec2((theta + step) / (2 * 3.14), 0.0), m_color));
		m_mesh->addVertex(VertexData(down_1, vec2(theta / (2 * 3.14), 0.0), m_color));
		m_mesh->addIndex(index + 2);
		m_mesh->addIndex(index + 1);
		m_mesh->addIndex(index);

		m_mesh->addVertex(VertexData(down_2, vec2((theta + step) / (2 * 3.14), 0.0), m_color));
		m_mesh->addVertex(VertexData(up_1, vec2(theta / (2 * 3.14), 1.0), m_color));
		m_mesh->addVertex(VertexData(up_2, vec2((theta + step) / (2 * 3.14), 1.0), m_color));
		m_mesh->addIndex(index + 5);
		m_mesh->addIndex(index + 4);
		m_mesh->addIndex(index + 3);

		float halfHeight = m_height / 2.0;

		//top
		vec3 centerTop = vec3(0.0, 0.0, halfHeight);
		m_mesh->addVertex(VertexData(up_1, circleUV(up_1, centerTop, m_radiusTop), m_color));
		m_mesh->addVertex(VertexData(up_2, circleUV(up_2, centerTop, m_radiusTop), m_color));
		m_mesh->addVertex(VertexData(centerTop, vec2(0.5, 0.5), m_color));
		m_mesh->addIndex(index + 0 + 5);
		m_mesh->addIndex(index + 1 + 5);
		m_mesh->addIndex(index + 2 + 5);

		//bottom
		vec3 centerBottom = vec3(0.0, 0.0, -halfHeight);
		m_mesh->addVertex(VertexData(down_2, circleUV(down_2, centerBottom, m_radiusBottom, true), m_color));
		m_mesh->addVertex(VertexData(down_1, circleUV(down_1, centerBottom, m_radiusBottom, true), m_color));
		m_mesh->addVertex(VertexData(centerBottom, vec2(0.5, 0.5), m_color));
		m_mesh->addIndex(index + 3 + 5);
		m_mesh->addIndex(index + 4 + 5);
		m_mesh->addIndex(index + 5 + 5);
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
