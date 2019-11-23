#include "ButtonPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"

#include "MainMenu.h"
#include "NodeEditorNodes/ButtonPartNode.h"
#include "3D/Model/Model.h"


namespace tzw
{
static auto blockSize = 0.25;

ButtonPart::ButtonPart(std::string itemName)
{
	m_topRadius = 0.1;
	m_bottomRadius = 0.1;
	m_height = 0.8;
	GamePart::initFromItemName(itemName);
	m_parent = nullptr;
	ButtonPart::generateName();

	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	m_graphNode = new ButtonPartNode(this);
	nodeEditor->addNode(m_graphNode);
}

ButtonPart::~ButtonPart()
{
	delete m_graphNode;
}

void ButtonPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, TRC(u8"°´Å¥ %s"),genShortName().c_str());
	setName(formatName);
}

GamePartType ButtonPart::getType()
{
	return GamePartType::GAME_PART_BUTTON;
}

void ButtonPart::drawInspect()
{
}

void ButtonPart::use()
{
}

bool ButtonPart::isNeedDrawInspect()
{
	return true;
}

GameNodeEditorNode* ButtonPart::getEditorNode()
{
	return m_graphNode;
}

void ButtonPart::onPressed()
{
	MainMenu::shared()->getNodeEditor()->onPressButtonNode(this->getEditorNode());
	static_cast<Model *>(m_node)->setCurrPose(0);
}

void ButtonPart::onReleased()
{
	MainMenu::shared()->getNodeEditor()->onReleaseSwitchNode(this->getEditorNode());
	static_cast<Model *>(m_node)->setCurrPose(-1);
}
}
