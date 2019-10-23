#include "BearPart.h"
#include "Interface/Drawable3D.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Texture/TextureMgr.h"
#include "NodeEditorNodes/BearingPartNode.h"
#include "MainMenu.h"
#include "GamePart.h"
#include "Island.h"
#include "Collision/PhysicsMgr.h"
#include "Utility/math/TbaseMath.h"

namespace tzw
{
	float blockSize = 0.10;
BearPart::BearPart()
{
	m_a = nullptr;
	m_b = nullptr;
	m_isFlipped = false;
	m_node = nullptr;
	m_constrain = nullptr;
	

	m_isSteering = false;
	m_isAngleLimit = false;
	m_angleLimitLow = -30.0f;
	m_angleLimitHigh = 30.0f;
	//forward backward
	m_attachment[0] = new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this);
	m_attachment[1] = new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this);
	BearPart::generateName();
	
	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	m_graphNode = new BearingPartNode(this);
	nodeEditor->addNode(m_graphNode);
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

int BearPart::getAttachmentCount()
{
	return 2;
}

BearPart::~BearPart()
{
	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	nodeEditor->removeNode(m_graphNode);
	delete m_graphNode;
}

GameNodeEditorNode* BearPart::getGraphNode() const
{
	return m_graphNode;
}

void BearPart::load(rapidjson::Value& constraintData)
{
	m_isFlipped = constraintData["isFlipped"].GetBool();
	updateFlipped();
    static_cast<ResNode *> (m_graphNode)->syncName();
}

void BearPart::setIsSteering(bool isSteering)
{
	m_isSteering = isSteering;
	if(m_constrain && m_isEnablePhysics)
	{
		if(m_isSteering)
		{
			m_constrain->enableAngularMotor(true, 0.0f, 10000.0f);
		}
		else 
		{
			m_constrain->enableAngularMotor(false, 0.0f, 10000.0f);
		}
	}

}

bool BearPart::getIsSteering() const
{
	return m_isSteering;
}

void BearPart::setAngleLimit(bool isAngleLimit, float low, float high)
{
	m_isAngleLimit = isAngleLimit;
	m_angleLimitLow = low;
	m_angleLimitHigh = high;
	if(m_constrain && m_isEnablePhysics)
	{
		if(m_isAngleLimit)
		{
			m_constrain->setLimit(m_angleLimitLow* (TbaseMath::PI_OVER_180), m_angleLimitHigh * (TbaseMath::PI_OVER_180));
		}
		else 
		{
			m_constrain->setLimit(0, 0);
		}
	}
}

void BearPart::getAngleLimit(bool& isAngleLimit, float& low, float& high) const
{
	isAngleLimit = m_isAngleLimit;
	low = m_angleLimitLow;
	high = m_angleLimitHigh;
}

void BearPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, u8"Öá³Ð %s",genShortName().c_str());
	setName(formatName);
}

	GamePartType BearPart::getType()
	{
		return GamePartType::GAME_PART_BEARING;
	}

	void BearPart::enableAngularMotor(bool enableMotor, float targetVelocity, float maxMotorImpulse)
	{
		if(m_constrain)
		{
			float factor = m_isFlipped?-1.0f:1.0f;
			m_constrain->enableAngularMotor(enableMotor, targetVelocity * factor, maxMotorImpulse);
		}
	}

void BearPart::findPiovtAndAxis(Attachment * attach, vec3 hingeDir,  vec3 & pivot, vec3 & asix)
{
	auto part = attach->m_parent;
	auto island = part->m_parent;
	auto islandInvertedMatrix = island->m_node->getLocalTransform().inverted();
	
	auto transform = part->getNode()->getLocalTransform();
	auto normalInIsland = transform.transofrmVec4(vec4(attach->m_normal, 0.0)).toVec3();

	pivot = transform.transofrmVec4(vec4(attach->m_pos + attach->m_normal * (blockSize / 2.0), 1.0)).toVec3();
	asix = islandInvertedMatrix.transofrmVec4(vec4(hingeDir, 0.0)).toVec3();
}

void BearPart::enablePhysics(bool isEnable)
{
	GameConstraint::enablePhysics(isEnable);
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
				vec3 hingeDir = (worldPosB - worldPosA).normalized();
				vec3 pivotA, pivotB, axisA, axisB;
				findPiovtAndAxis(attachA, hingeDir, pivotA, axisA);
				findPiovtAndAxis(attachB, hingeDir, pivotB, axisB);

				auto constrain = PhysicsMgr::shared()->createHingeConstraint(partA->m_parent->m_rigid, partB->m_parent->m_rigid, pivotA, pivotB, axisA, axisB, false);
				m_constrain = constrain;

			}
		}
		else 
		{
			PhysicsMgr::shared()->addConstraint(m_constrain);
		}
		if(m_isSteering)
		{
			m_constrain->enableAngularMotor(true, 0.0f, 10000.0f);
		}

		if(m_isAngleLimit)
		{
			m_constrain->setLimit(m_angleLimitLow* (TbaseMath::PI_OVER_180), m_angleLimitHigh * (TbaseMath::PI_OVER_180));
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
	GameConstraint::dump(partData, allocator);
	partData.AddMember("Type", "Bearing", allocator);
	
	partData.AddMember("from", std::string(m_b->getGUID()), allocator);

	partData.AddMember("isSteering", getIsSteering(), allocator);

	bool isLimit;
	float low,high;
	getAngleLimit(isLimit, low, high);
	partData.AddMember("isAngleLimit", isLimit, allocator);
	partData.AddMember("AngleLimitLow", low, allocator);
	partData.AddMember("AngleLimitHigh", high, allocator);
	if(m_a)
	{
		partData.AddMember("to", std::string(m_a->getGUID()), allocator);
	}
	rapidjson::Value attachList(rapidjson::kArrayType);
	int count = getAttachmentCount();
	for(int k = 0; k < count; k++)
	{
		auto attach = getAttachment(k);
		rapidjson::Value attachObj(rapidjson::kObjectType);
		attachObj.AddMember("UID", std::string(attach->getGUID()), allocator);
		attachList.PushBack(attachObj, allocator);
		
	}
	partData.AddMember("attachList", attachList, allocator);
	partData.AddMember("UID", std::string(getGUID()), allocator);
	partData.AddMember("isFlipped", m_isFlipped, allocator);
}

Attachment* BearPart::getFirstAttachment()
{
	return m_attachment[0];
}

Attachment* BearPart::getAttachment(int index)
{
	return m_attachment[index];
}
}

