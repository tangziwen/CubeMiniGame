#include "ThrusterPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "MainMenu.h"
#include "NodeEditorNodes/ThrusterPartNode.h"
#include "Utility/math/TbaseMath.h"
#include <ctime>
#include "BuildingSystem.h"


namespace tzw
{
ThrusterPart::ThrusterPart()
{

	m_topRadius = 0.1;
	m_bottomRadius = 0.25;
	m_height = 0.5;
	m_node = new CylinderPrimitive(m_topRadius, m_bottomRadius, m_height);
	m_isOpen = 0;
	m_shape = new PhysicsShape();
	m_shape->initCylinderShapeZ(m_bottomRadius, m_topRadius, m_height);
	m_parent = nullptr;
	ThrusterPart::generateName();

	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	m_graphNode = new ThrusterPartNode(this);
	nodeEditor->addNode(m_graphNode);
	initAttachments();
	m_phase = TbaseMath::randF()*3.14;
	m_scale =0.8f + TbaseMath::randF() * 0.3f;
	m_dir_t = 0.0f;
	m_t = 0.0f;
	m_phaseV3 = vec3(TbaseMath::randFN()*1000.0f, TbaseMath::randF()*1000.0f, TbaseMath::randF()*1000.0f);
	flatNoise.SetSeed(time(nullptr));
	BuildingSystem::shared()->addThruster(this);
}

void ThrusterPart::initAttachments()
{
	//forward backward
	m_attachment[0] = new Attachment(vec3(0.0, 0.0, m_height / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this);
}

Attachment * ThrusterPart::getAttachmentInfo(int index, vec3 & pos, vec3 & N, vec3 & up)
{
	auto mat = m_node->getLocalTransform();
	auto atta = m_attachment[index];
	vec4 a_pos = vec4(atta->m_pos, 1.0);
	vec4 a_n = vec4(atta->m_normal, 0.0);
	vec4 a_up = vec4(atta->m_up, 0.0);
	pos = mat.transofrmVec4(a_pos).toVec3();
	N = mat.transofrmVec4(a_n).toVec3();
	up = mat.transofrmVec4(a_up).toVec3();
	return m_attachment[index];
}

Attachment* ThrusterPart::getFirstAttachment()
{
	return m_attachment[0];
}

Attachment* ThrusterPart::getBottomAttachment()
{
	int theSmallIndex = -1;
	float smallDist = 99999.0f;
	for(int i =0; i < 6; i++) 
	{
		vec3 pos, n, up;
		getAttachmentInfo(i, pos, n, up);
		if(pos.y < smallDist) 
		{
			smallDist = pos.y;
			theSmallIndex = i;
		}
		
	}
	return m_attachment[theSmallIndex];
}

Attachment* ThrusterPart::getAttachment(int index)
{
	return m_attachment[index];
}

int ThrusterPart::getAttachmentCount()
{
	return 1;
}

void ThrusterPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, u8"ÍÆ½øÆ÷ %s",genShortName().c_str());
	setName(formatName);
}

GamePartType ThrusterPart::getType()
{
	return GamePartType::GAME_PART_THRUSTER;
}

void ThrusterPart::toggle(int openSignal)
{
	 
	m_isOpen = openSignal;

}

void ThrusterPart::updateForce(float dt)
{
	if(m_isOpen)
	{
		if(m_parent->m_rigid)
		{
			m_t += dt * 6;
			m_dir_t += dt * 3.0f;
			m_parent->m_rigid->activate();
			auto randomDir = vec3(flatNoise.GetValue(m_scale * m_dir_t + m_phaseV3.x,0, 0),
				flatNoise.GetValue(m_scale * m_dir_t + m_phaseV3.y,0, 0),
				flatNoise.GetValue(m_scale * m_dir_t + m_phaseV3.z,0, 0)) * 0.08;
			auto forceDir = m_node->getForward() *-1 + randomDir;
			auto forceVariation = sinf(m_phase + m_scale * m_t) * 0.3;
			m_parent->m_rigid->applyForce(forceDir * (25.0f  + forceVariation), m_node->getPos());
		}		
	}
	//apply recoil
}

GameNodeEditorNode* ThrusterPart::getGraphNode() const
{
	return m_graphNode;
}

ThrusterPart::~ThrusterPart()
{
	BuildingSystem::shared()->removeThruster(this);
}
}
