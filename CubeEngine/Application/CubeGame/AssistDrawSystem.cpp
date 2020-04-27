#include "AssistDrawSystem.h"
#include "Scene/SceneMgr.h"
#include "BuildingSystem.h"
#include "2D/LabelNew.h"
#include "3D/Primitive/LinePrimitive.h"

namespace tzw
{

AssistDrawSystem::AssistDrawSystem()
{
	m_isShowAssistInfo = false;
	m_line = new LinePrimitive();
}

void AssistDrawSystem::handleDraw(float dt)
{
	if(!m_isShowAssistInfo) return;
	auto constraintList = BuildingSystem::shared()->getConstraintList();
	for(auto constrain : constraintList)
	{
		auto label = getOrCreate(constrain);
		auto posIn2D = g_GetCurrScene()->defaultCamera()->worldToScreen(constrain->getWorldPos());
		label->setPos2D(posIn2D.xy());
	}
}

bool AssistDrawSystem::getIsShowAssistInfo()
{
	return m_isShowAssistInfo;
}

void AssistDrawSystem::setIsShowAssistInfo(bool newVal)
{
	if(!newVal && m_isShowAssistInfo)
	{
		setShowAllPartLabel(false);
	}

	if(newVal && !m_isShowAssistInfo)
	{
		setShowAllPartLabel(true);
	}
	m_isShowAssistInfo = newVal;
}

LabelNew* AssistDrawSystem::getOrCreate(GamePart* part)
{
	if(m_partLabelMap.find(part) == m_partLabelMap.end())
	{
		auto m_label = LabelNew::create(part->getName());
		g_GetCurrScene()->addNode(m_label);
		m_partLabelMap[part] = m_label;
		return m_label;
	}else
	{
		auto label = m_partLabelMap[part];
		//name has been changed
		if(label->getString() != part->getName())
		{
			label->setString(part->getName());
		}
		return m_partLabelMap[part];
	}
}

void AssistDrawSystem::setShowAllPartLabel(bool isShow)
{
	for(auto i : m_partLabelMap)
	{
		i.second->setIsVisible(isShow);
	}
}

void AssistDrawSystem::drawBoundingBox(AABB aabb, Matrix44 mat)
{
	static bool isFirstTime = true;
	if(!isFirstTime)
	{
		return;;
	}
	
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
	corners[7] = mat.transformVec3(vec3(minV.x, minV.y, minV.z));
	for(int i =0; i< 8; i++)
	{
		for(int j = i + 1; j < 8; j++)
		{
			m_line->append(corners[i], corners[j]);
		}
	}
	m_line->initBuffer();
	g_GetCurrScene()->addNode(m_line);
	isFirstTime = false;
}
}
