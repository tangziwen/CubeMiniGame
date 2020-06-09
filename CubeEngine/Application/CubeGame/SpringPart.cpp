#include "SpringPart.h"
#include "Interface/Drawable3D.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Texture/TextureMgr.h"
#include "NodeEditorNodes/BearingPartNode.h"
#include "GameUISystem.h"
#include "GamePart.h"
#include "Island.h"
#include "Collision/PhysicsMgr.h"
#include "NodeEditorNodes/SpringPartNode.h"
#include "Base/TranslationMgr.h"

namespace tzw
{
SpringPart::SpringPart(std::string itemName)
{
	m_a = nullptr;
	m_b = nullptr;
	m_node = nullptr;
	m_constrain = nullptr;
	GamePart::initFromItemName(itemName);
	m_stiffness = 600.0f;
	m_damping = 0.8f;
	//forward backward
	float blockSize = 0.50f;
	addAttachment(new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this));
	addAttachment(new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this));
	// create a indicate model
	auto cylinderIndicator = new CylinderPrimitive(0.15, 0.15, 0.5);
	cylinderIndicator->setColor(vec4(1.0, 1.0, 0.0, 0.0));
	cylinderIndicator->reCache();
	setNode(cylinderIndicator);
	SpringPart::generateName();
}

void SpringPart::findPiovtAndAxis(Attachment * attach, vec3 hingeDir,  vec3 & pivot, vec3 & asix)
{
	auto part = attach->m_parent;
	auto island = part->m_parent;
	auto islandInvertedMatrix = island->m_node->getLocalTransform().inverted();
	
	auto transform = part->getNode()->getLocalTransform();
	auto normalInIsland = transform.transofrmVec4(vec4(attach->m_normal, 0.0)).toVec3();

	pivot = transform.transofrmVec4(vec4(attach->m_pos + attach->m_normal * 0.05, 1.0)).toVec3();
	asix = islandInvertedMatrix.transofrmVec4(vec4(hingeDir, 0.0)).toVec3();
}

static Matrix44 groupMatNode(vec3 pos, vec3 normal, vec3 up, Matrix44 reservedMat)
{
	Matrix44 mat;
	auto data = mat.data();
	vec3 right = vec3::CrossProduct(normal * -1, up);
	data[0] = right.x;
	data[1] = right.y;
	data[2] = right.z;
	data[3] = 0.0;

	data[4] = up.x;
	data[5] = up.y;
	data[6] = up.z;
	data[7] = 0.0;

	data[8] = normal.x;
	data[9] = normal.y;
	data[10] = normal.z;
	data[11] = 0.0;

	data[12] = pos.x;
	data[13] = pos.y;
	data[14] = pos.z;
	data[15] = 1.0;

	return reservedMat * mat;
}
void SpringPart::enablePhysics(bool isEnable)
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
				updateConstraintState();
				PhysicsMgr::shared()->addConstraint(m_constrain);
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

void SpringPart::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
	partData.AddMember("Type", "Spring", allocator);
	partData.AddMember("from", std::string(m_b->getGUID()), allocator);
	if(m_a)
	{
		partData.AddMember("to", std::string(m_a->getGUID()), allocator);
	}
	dumpAttach(partData, allocator);
	partData.AddMember("UID", std::string(getGUID()), allocator);
}

Attachment* SpringPart::getFirstAttachment()
{
	return m_attachment[0];
}

Attachment* SpringPart::getAttachment(int index)
{
	return m_attachment[index];
}

int SpringPart::getAttachmentCount()
{
	return 2;
}

float SpringPart::getStiffness() const
{
	return m_stiffness;
}

float SpringPart::getDamping() const
{
	return m_damping;
}

void SpringPart::setStiffness(float stiffness)
{
	m_stiffness = stiffness;
	if(m_constrain && m_isEnablePhysics)
	{
		m_constrain->setStiffness(2, m_stiffness);
	}
}

void SpringPart::setDamping(float damping)
{
	m_damping = damping;
	if(m_constrain && m_isEnablePhysics)
	{
		m_constrain->setDamping(2, m_damping);
	}
}

GraphNode* SpringPart::getGraphNode() const
{
	return m_graphNode;
}

void SpringPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, TRC(u8"µ¯»É %s"),genShortName().c_str());
	setName(formatName);
}

SpringPart::~SpringPart()
{
	auto nodeEditor = GameUISystem::shared()->getNodeEditor();
	nodeEditor->removeNode(m_graphNode);
	delete m_graphNode;
	if(m_constrain) 
	{
		PhysicsMgr::shared()->removeConstraint(m_constrain);
		delete m_constrain;
	}
}

void SpringPart::drawInspect()
{
	if(drawInspectNameEdit())
	{
		static_cast<ResNode *>(m_graphNode)->syncName();
	}
	float stiffness, damping;
	stiffness = getStiffness();
	damping = getDamping();
	bool isInputStiffness = false, isInputDamping = false;
	isInputStiffness = ImGui::InputFloat(TRC(u8"¾¢¶ÈÏµÊý"), &stiffness);
	if(isInputStiffness)
	{
		setStiffness(stiffness);
	}
	isInputDamping = ImGui::InputFloat(TRC(u8"×èÄá"), &damping);
	if(isInputDamping)
	{
		setStiffness(damping);
	}
}

bool SpringPart::isNeedDrawInspect()
{
	return true;
}

void SpringPart::updateConstraintState()
{
	if(m_constrain)
	{
		if(m_isEnablePhysics)
		{
			PhysicsMgr::shared()->removeConstraint(m_constrain);
		}
		delete m_constrain;
	}

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
		vec3 center = (worldPosB + worldPosA) * 0.5f;
		Matrix44 frameInA;
		tlog("the DIst%f", worldPosA.distance(worldPosB));
		frameInA = setUpFrameFromZ(center, hingeDir, partA->m_parent->m_node->getTransform().inverted());
		Matrix44 frameInB;
		frameInB = setUpFrameFromZ(center, hingeDir, partB->m_parent->m_node->getTransform().inverted());
		auto constraint = PhysicsMgr::shared()->create6DOFSprintConstraint(partA->m_parent->m_rigid, partB->m_parent->m_rigid, frameInA, frameInB);
		constraint->enableSpring(2, true);
		constraint->setLinearLowerLimit(vec3(0, 0, 1));
		constraint->setLinearUpperLimit(vec3(0, 0, -1));
		constraint->setAngularLowerLimit(vec3(0, 0, 0));
		constraint->setAngularUpperLimit(vec3(0, 0, 0));
		constraint->setStiffness(2, 600.0f);
		constraint->setDamping(2, 0.8f);
		constraint->setEquilibriumPoint(2, 0);
		m_constrain = constraint;
		tlog("what the fuck %f", constraint->getOffset());
	}
	if(m_isEnablePhysics)
	{
		PhysicsMgr::shared()->addConstraint(m_constrain);
	}
}
}

