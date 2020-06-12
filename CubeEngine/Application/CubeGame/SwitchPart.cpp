#include "SwitchPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "NodeEditorNodes/SwitchNode.h"
#include "GameUISystem.h"
#include "3D/Model/Model.h"


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
	m_currState = false;

}

SwitchPart::~SwitchPart()
{
	auto nodeEditor = m_vehicle->getEditor();
	nodeEditor->removeNode(m_graphNode);
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

void SwitchPart::onToggle()
{
	m_currState = !m_currState;
	m_vehicle->getEditor()->onReleaseSwitchNode(this->getGraphNode());
	static_cast<Model *>(m_node)->setCurrPose(m_currState?0:-1);
	Texture * tex;
	if(m_currState)
	{
		tex = TextureMgr::shared()->getByPath("Blocks/Switch/SwitchOn_diffuse.png");
	}
	else
	{
		tex = TextureMgr::shared()->getByPath("Blocks/Switch/SwitchOff_diffuse.png");
	}
	static_cast<Model *>(m_node)->getMat(0)->setTex("DiffuseMap", tex);
	
}

void SwitchPart::AddOnVehicle(Vehicle* vehicle)
{
	m_graphNode = new SwitchNode(this);
	vehicle->getEditor()->addNode(m_graphNode);
}

bool SwitchPart::isCurrState() const
{
	return m_currState;
}

void SwitchPart::setCurrState(const bool currState)
{
	m_currState = currState;
}
}
