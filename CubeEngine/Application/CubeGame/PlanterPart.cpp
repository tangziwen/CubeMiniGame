#include "PlanterPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"

#include "GameUISystem.h"
#include "NodeEditorNodes/PlanterPartNode.h"
#include "3D/Model/Model.h"
#include "CropSystem.h"


namespace tzw
{
static auto blockSize = 0.25;

PlanterPart::PlanterPart(std::string itemName)
{
	m_topRadius = 0.1;
	m_bottomRadius = 0.1;
	m_height = 0.8;
	GamePart::initFromItemName(itemName);
	m_parent = nullptr;
	PlanterPart::generateName();
}

PlanterPart::~PlanterPart()
{
	auto nodeEditor = m_vehicle->getEditor();
	nodeEditor->removeNode(m_graphNode);
	delete m_graphNode;
}

void PlanterPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, TRC(u8"Planter %s"),genShortName().c_str());
	setName(formatName);
}

GamePartType PlanterPart::getType()
{
	return GamePartType::GAME_PART_PLANTER;
}

void PlanterPart::drawInspect()
{
}

void PlanterPart::use()
{
	CropSystem::shared()->getCropByName("Wheat")->plantCrop(getWorldPos());
}

bool PlanterPart::isNeedDrawInspect()
{
	return true;
}


void PlanterPart::onPressed()
{
	m_vehicle->getEditor()->onPressButtonNode(this->getGraphNode());
	static_cast<Model *>(m_node)->setCurrPose(0);
}

void PlanterPart::onReleased()
{
	m_vehicle->getEditor()->onReleaseSwitchNode(this->getGraphNode());
	static_cast<Model *>(m_node)->setCurrPose(-1);
}

void PlanterPart::AddOnVehicle(Vehicle* vehicle)
{
	m_graphNode = new PlanterPartNode(this);
	vehicle->getEditor()->addNode(m_graphNode);
}
}
