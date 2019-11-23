#include "SwitchPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "NodeEditorNodes/CannonPartNode.h"
#include "MainMenu.h"


namespace tzw
{
static auto blockSize = 0.25;

SwitchPart::SwitchPart(std::string itemName)
{
	m_topRadius = 0.1;
	m_bottomRadius = 0.1;
	m_height = 0.8;
	GamePart::initFromItemName(itemName);
	m_parent = nullptr;
	SwitchPart::generateName();
}

SwitchPart::~SwitchPart()
{
	delete m_graphNode;
}

void SwitchPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, TRC(u8"¿ª¹Ø %s"),genShortName().c_str());
	setName(formatName);
}

GamePartType SwitchPart::getType()
{
	return GamePartType::GAME_PART_SWITCH;
}

void SwitchPart::drawInspect()
{
}

void SwitchPart::use()
{
}

bool SwitchPart::isNeedDrawInspect()
{
	return true;
}

}
