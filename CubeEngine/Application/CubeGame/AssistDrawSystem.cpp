#include "AssistDrawSystem.h"
#include "Scene/SceneMgr.h"
#include "BuildingSystem.h"
#include "2D/LabelNew.h"

namespace tzw
{
TZW_SINGLETON_IMPL(AssistDrawSystem);
AssistDrawSystem::AssistDrawSystem()
{
	m_isShowAssistInfo = false;
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
}
