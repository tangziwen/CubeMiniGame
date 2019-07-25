#include "BearPart.h"
#include "Interface/Drawable3D.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Texture/TextureMgr.h"
#include "NodeEditorNodes/BearingPartNode.h"
#include "MainMenu.h"
#include "GamePart.h"
#include "Island.h"
#include "Collision/PhysicsMgr.h"

namespace tzw
{
BearPart::BearPart()
{
	m_a = nullptr;
	m_b = nullptr;
	m_isFlipped = false;
	m_node = nullptr;
	m_constrain = nullptr;
	
	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	auto node = new BearingPartNode(this);
	nodeEditor->addNode(node);
}

void BearPart::updateFlipped()
{
	if(!m_node) return;
	auto cylinder = static_cast<CylinderPrimitive *>(m_node);
	if(m_isFlipped)
	{
		cylinder->setTopBottomTex(TextureMgr::shared()->getByPath("Texture/bear_flipped.png"));
	}
	else
	{
		cylinder->setTopBottomTex(TextureMgr::shared()->getByPath("Texture/bear.png"));
	}
}
void BearPart::findPiovtAndAxis(Attachment * attach, vec3 hingeDir,  vec3 & pivot, vec3 & asix)
{
	auto part = attach->m_parent;
	auto island = part->m_parent;
	auto islandInvertedMatrix = island->m_node->getLocalTransform().inverted();
	
	auto transform = part->getNode()->getLocalTransform();
	auto normalInIsland = transform.transofrmVec4(vec4(attach->m_normal, 0.0)).toVec3();

	pivot = transform.transofrmVec4(vec4(attach->m_pos + attach->m_normal * 0.05, 1.0)).toVec3();
	asix = islandInvertedMatrix.transofrmVec4(vec4(hingeDir, 0.0)).toVec3();
}

void BearPart::enablePhysics(bool isEnable)
{
	if(isEnable) 
	{
		if(!m_constrain) 
		{
		auto attachA = m_a;
		auto attachB = m_b;
		if (attachA && attachB) 
		{
			auto partA = attachA->m_parent;
			auto partB = attachB->m_parent;

			vec3 worldPosA, worldNormalA, worldUpA;
			attachA->getAttachmentInfoWorld(worldPosA, worldNormalA, worldUpA);
			vec3 worldPosB, worldNormalB, worldUpB;
			attachB->getAttachmentInfoWorld(worldPosB, worldNormalB, worldUpB);
			int isFlipped = m_isFlipped ? -1 : 1;
			vec3 hingeDir = (worldPosB - worldPosA).normalized() * isFlipped;
			vec3 pivotA, pivotB, axisA, axisB;
			findPiovtAndAxis(attachA, hingeDir, pivotA, axisA);
			findPiovtAndAxis(attachB, hingeDir, pivotB, axisB);

			auto constrain = PhysicsMgr::shared()->createHingeConstraint(partA->m_parent->m_rigid, partB->m_parent->m_rigid, pivotA, pivotB, axisA, axisB, false);
			m_constrain = constrain;
		}
		} else 
		{
			PhysicsMgr::shared()->addConstraint(m_constrain);
		}
	} else 
	{
		if(m_constrain) 
		{
			PhysicsMgr::shared()->removeConstraint(m_constrain);
		}
	}
}

void BearPart::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
	partData.AddMember("Type", "Bearing", allocator);
	partData.AddMember("from", std::string(m_b->getGUID()), allocator);
	if(m_a)
	{
		partData.AddMember("to", std::string(m_a->getGUID()), allocator);
	}
}
}

