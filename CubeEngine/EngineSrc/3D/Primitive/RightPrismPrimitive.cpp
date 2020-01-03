#include "RightPrismPrimitive.h"
#include <algorithm>
#include "../../Rendering/RenderCommand.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "../EngineSrc/Collision/CollisionUtility.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Collision/PhysicsMgr.h"
namespace tzw {

RightPrismPrimitive::RightPrismPrimitive(float width,  float height, float depth, bool isNeedPreGenerateMat)
{
    m_width = width;
	m_depth = depth;
	m_height = height;
	m_mesh = nullptr;
	m_color = vec4::fromRGB(255,255,255);
	if(isNeedPreGenerateMat)
	{
	    m_material = Material::createFromTemplate("ModelSTD");
		auto texture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_basecolor.png");
		m_material->setTex("DiffuseMap", texture);
	}
	else
	{
		m_material = nullptr;
	}

    initMesh();
    setCamera(g_GetCurrScene()->defaultCamera());
    setIsAccpectOcTtree(true);
}

void RightPrismPrimitive::submitDrawCmd(RenderCommand::RenderType passType)
{
	if(getIsVisible())
	{
		RenderCommand command(m_mesh, m_material,passType);
		setUpCommand(command);
		Renderer::shared()->addRenderCommand(command);
	}
}

bool RightPrismPrimitive::intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint)
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

void RightPrismPrimitive::setColor(vec4 color)
{
	m_color = color;
	initMesh();
}

bool RightPrismPrimitive::isHit(Ray ray)
{
	auto invertedMat = getTransform().inverted();
	vec4 dirInLocal = invertedMat * vec4(ray.direction(), 0.0);
	vec4 originInLocal = invertedMat * vec4(ray.origin(), 1.0);

	auto r = Ray(originInLocal.toVec3(), dirInLocal.toVec3());
	RayAABBSide side;
	vec3 hitPoint;
	auto isHit = r.intersectAABB(localAABB(), &side, hitPoint);
	if (isHit)
	{
		return true;
	}
	return false;
}

Mesh* RightPrismPrimitive::getMesh()
{
	return m_mesh;
}

Mesh* RightPrismPrimitive::getMesh(int index)
{
	return m_mesh;
}

vec3 RightPrismPrimitive::getWorldPos(vec3 localPos)
{
	auto theMat = getTransform();
	return theMat.transformVec3(localPos);
}

void RightPrismPrimitive::initMesh()
{
    auto halfWidth = m_width/2.0f;
	auto halfDepth = m_depth/2.0f;
	auto halfHeight = m_height/2.0f;
	if (!m_mesh)
	{
		m_mesh = new Mesh();
	}
	else
	{
		m_mesh->clear();
	}
    VertexData vertices[] = {
        // front
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.0f), m_color),  // v0
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(1.f, 0.0f), m_color), // v1
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 1.0f), m_color),  // v2
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(1.f, 1.f), m_color), // v3

        // bottom
        VertexData(vec3( -1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2( 0.0f, 1.0f), m_color), // v4
        VertexData(vec3( -1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.0f, 0.0f), m_color), // v5
        VertexData(vec3( 1.0f *halfWidth,  -1.0f * halfHeight,  -1.0f * halfDepth), vec2(1.0f, 0.0f), m_color),  // v6
        VertexData(vec3( 1.0f *halfWidth,  -1.0f * halfHeight, 1.0f * halfDepth), vec2(1.0f, 1.0f), m_color), // v7

        // left half
        VertexData(vec3( -1.0f *halfWidth, 1.0f * halfHeight, 1.0f * halfDepth), vec2(0.66f, 0.5f), m_color), // v8
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 0.5f), m_color),  // v9
        VertexData(vec3( -1.0f *halfWidth,  -1.0f * halfHeight, 1.0f * halfDepth), vec2(0.66f, 1.0f), m_color), // v10

    	
        // right half
        VertexData(vec3( 1.0f *halfWidth, 1.0f * halfWidth, 1.0f * halfDepth), vec2(0.66f, 0.5f), m_color), // v11
        VertexData(vec3(1.0f *halfWidth, -1.0f * halfWidth, 1.0f * halfDepth), vec2(1.0f, 0.5f), m_color),  // v12
        VertexData(vec3( 1.0f *halfWidth,  -1.0f * halfWidth, -1.0f * halfDepth), vec2(0.66f, 1.0f), m_color), // v13

        // bevel
        VertexData(vec3(-1.0f *halfWidth, 1.0f * halfWidth,  1.0f * halfDepth), vec2(1.0f, 0.0f), m_color),  // v14
        VertexData(vec3( 1.0f *halfWidth, 1.0f * halfWidth,  1.0f * halfDepth), vec2(0.0f, 0.0f), m_color), // v15
        VertexData(vec3(1.0f *halfWidth,  -1.0f * halfWidth,  -1.0f * halfDepth), vec2(0.0f, 1.0f), m_color),  // v16
        VertexData(vec3( -1.0f *halfWidth,  -1.0f * halfWidth,  -1.0f * halfDepth), vec2(1.0f, 1.0f), m_color), // v17
    };


	unsigned short indices[] = {
         0,  1,  2,  1,  3,  2, //front
         4,  5,  6,  4,  6,  7,  //bottom
         8,  9,  10, 11, 12, 13, // left & right half
		14,  15,  16,  14,  16,  17, //bevel
    };

    m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
    m_mesh->caclNormals();
    m_mesh->finish();
    m_localAABB.merge(m_mesh->getAabb());
    reCache();
    reCacheAABB();
}

void RightPrismPrimitive::checkCollide(ColliderEllipsoid * package)
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

} // namespace tzw
