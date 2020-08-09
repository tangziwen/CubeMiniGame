#include "DebugSystem.h"
#include "Scene/SceneMgr.h"
#include "2D/LabelNew.h"
#include "3D/Primitive/LinePrimitive.h"

namespace tzw
{

DebugSystem::DebugSystem()
{
	m_line = new LinePrimitive();
}

void DebugSystem::handleDraw(float dt)
{
	m_line->clear();
	
}

void DebugSystem::doRender(float dt)
{
	if(m_line->getLineCount() > 0)
	{
		m_line->initBuffer();
		m_line->submitDrawCmd(RenderFlag::RenderStage::COMMON);
	}
}


void DebugSystem::drawBoundingBox(AABB aabb, Matrix44 mat)
{
	
	vec3 minV = aabb.min();
	vec3 maxV = aabb.max();
	
	//get corners
	vec3 corners[8];
	corners[0] = mat.transformVec3(minV);
	corners[1] = mat.transformVec3(vec3(maxV.x, minV.y, minV.z));
	corners[2] = mat.transformVec3(vec3(maxV.x, maxV.y, minV.z));
	corners[3] = mat.transformVec3(vec3(minV.x, maxV.y, minV.z));
	corners[4] = mat.transformVec3(vec3(minV.x, minV.y, maxV.z));
	corners[5] = mat.transformVec3(vec3(maxV.x, minV.y, maxV.z));
	corners[6] = mat.transformVec3(maxV);
	corners[7] = mat.transformVec3(vec3(minV.x, maxV.y, maxV.z));
	//for(int i =0; i< 8; i++)
	//{
	//	for(int j = i + 1; j < 8; j++)
	//	{
	//		m_line->append(corners[i], corners[j]);
	//	}
	//}
	m_line->append(corners[0], corners[1]);
	m_line->append(corners[0], corners[4]);
	m_line->append(corners[0], corners[3]);

	m_line->append(corners[3], corners[2]);
	m_line->append(corners[3], corners[7]);

	m_line->append(corners[6], corners[2]);
	m_line->append(corners[6], corners[7]);
	m_line->append(corners[6], corners[5]);

	m_line->append(corners[5], corners[4]);
	m_line->append(corners[5], corners[1]);

	m_line->append(corners[1], corners[2]);

	m_line->append(corners[4], corners[7]);

}

void DebugSystem::drawPointCross(vec3 pointInWorld)
{
	float pointSize = 0.1;
	m_line->append(pointInWorld + vec3(-pointSize, 0, 0), pointInWorld + vec3(pointSize, 0, 0));
	m_line->append(pointInWorld + vec3(0, -pointSize, 0), pointInWorld + vec3(0, pointSize, 0));
	m_line->append(pointInWorld + vec3(0, 0, -pointSize), pointInWorld + vec3(0, 0, pointSize));
}

void DebugSystem::drawLine(vec3 A, vec3 B)
{
	m_line->append(A, B, vec3(1, 0, 0));
}
}
