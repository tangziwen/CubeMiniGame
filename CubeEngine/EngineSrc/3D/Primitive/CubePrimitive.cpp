#include "CubePrimitive.h"
#include <algorithm>
#include "../../Rendering/RenderCommand.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "../EngineSrc/Collision/CollisionUtility.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Collision/PhysicsMgr.h"
namespace tzw {

CubePrimitive::CubePrimitive(float width, float depth, float height, bool isNeedPreGenerateMat)
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

void CubePrimitive::submitDrawCmd(RenderCommand::RenderType passType)
{
	if(getIsVisible())
	{
		RenderCommand command(m_mesh, m_material,passType);
		setUpCommand(command);
		Renderer::shared()->addRenderCommand(command);
	}
}

bool CubePrimitive::intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint)
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

void CubePrimitive::setColor(vec4 color)
{
	m_color = color;
	initMesh();
}

bool CubePrimitive::isHit(Ray ray)
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

Mesh* CubePrimitive::getMesh()
{
	return m_mesh;
}

Mesh* CubePrimitive::getMesh(int index)
{
	return m_mesh;
}

vec3 CubePrimitive::getWorldPos(vec3 localPos)
{
	auto theMat = getTransform();
	return theMat.transformVec3(localPos);
}

void CubePrimitive::initMesh()
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
        // Vertex data for face 0
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.0f), m_color),  // v0
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.0f), m_color), // v1
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.5f), m_color),  // v2
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.5f), m_color), // v3

        // Vertex data for face 1
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2( 0.0f, 0.5f), m_color), // v4
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 0.5f), m_color), // v5
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 1.0f), m_color),  // v6
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 1.0f), m_color), // v7

        // Vertex data for face 2
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.5f), m_color), // v8
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 0.5f), m_color),  // v9
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 1.0f), m_color), // v10
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 1.0f), m_color),  // v11

        // Vertex data for face 3
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.0f), m_color), // v12
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 0.0f), m_color),  // v13
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.5f), m_color), // v14
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 0.5f), m_color),  // v15

        // Vertex data for face 4
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 0.0f), m_color), // v16
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.0f), m_color), // v17
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.5f), m_color), // v18
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.66f, 0.5f), m_color), // v19

        // Vertex data for face 5
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.5f), m_color), // v20
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.66f, 0.5f), m_color), // v21
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 1.0f), m_color), // v22
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 1.0f), m_color)  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
	unsigned short indices[] = {
         0,  1,  2,  1,  3,  2,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  5,  6,  5,  7,  6, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  9,  10, 9, 11, 10, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 13, 14, 13, 15, 14, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 17, 18, 17, 19, 18, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 21, 22, 21, 23, 22,      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
    m_mesh->caclNormals();
    m_mesh->finish();
    m_localAABB.merge(m_mesh->getAabb());
    reCache();
    reCacheAABB();
}

void CubePrimitive::checkCollide(ColliderEllipsoid * package)
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
