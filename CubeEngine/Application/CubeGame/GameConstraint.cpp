#include "GameConstraint.h"
#include "GamePart.h"

namespace tzw
{
GameConstraint::GameConstraint()
{
	m_a = nullptr;
	m_b = nullptr;
}

void GameConstraint::enablePhysics(bool isEnable)
{

}

void GameConstraint::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
}

bool GameConstraint::isConstraint()
{
	return true;
}
}

