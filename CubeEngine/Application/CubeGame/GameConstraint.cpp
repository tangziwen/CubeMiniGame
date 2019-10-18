#include "GameConstraint.h"
#include "GamePart.h"
#include "2D/LabelNew.h"
#include "Scene/SceneMgr.h"

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
		attachToFromOtherIslandAlterSelfIsland(m_b);
	}
}

void GameConstraint::setName(std::string newName)
{
	GamePart::setName(newName);
	if(m_label)
	{
		m_label->setString(newName);
	}
}

GameConstraint::~GameConstraint()
{
	if(m_label)
	{
		m_label->removeFromParent();
		delete m_label;
	}
}
}

