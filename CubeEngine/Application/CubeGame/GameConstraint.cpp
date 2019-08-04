#include "GameConstraint.h"
#include "GamePart.h"

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
}

