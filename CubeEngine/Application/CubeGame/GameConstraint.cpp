#include "GameConstraint.h"
#include "GamePart.h"
#include "2D/LabelNew.h"
#include "Scene/SceneMgr.h"
#include "GameItem.h"

namespace tzw
{
GameConstraint::GameConstraint()
{
	m_a = nullptr;
	m_b = nullptr;
	m_isEnablePhysics = false;
}

void GameConstraint::enablePhysics(bool isEnable)
{
	m_isEnablePhysics = isEnable;
}

void GameConstraint::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
	partData.AddMember("Name", m_name, allocator);
	partData.AddMember("ItemName", m_item->m_name, allocator);
}

void GameConstraint::load(rapidjson::Value& partData)
{
	
}

bool GameConstraint::isConstraint()
{
	return true;
}

void GameConstraint::updateTransform(float dt)
{
	if(m_isEnablePhysics)
	{
		for(int i = 0; i < getAttachmentCount(); i++)
		{
			auto attach = getAttachment(i);
			if(attach->m_connected)
			{
				adjustToOtherIslandByAlterSelfIsland(attach->m_connected, attach, attach->m_degree);
				break;
			}
		}
	}
}

void GameConstraint::setName(std::string newName)
{
	GamePart::setName(newName);
}

GameConstraint::~GameConstraint()
{
}

void GameConstraint::updateConstraintState()
{
	
}
}

