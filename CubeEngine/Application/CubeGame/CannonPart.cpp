#include "CannonPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "NodeEditorNodes/CannonPartNode.h"
#include "MainMenu.h"
#include "3D/Primitive/CubePrimitive.h"


namespace tzw
{
CannonPart::CannonPart()
{

	m_topRadius = 0.1;
	m_bottomRadius = 0.1;
	m_height = 0.8;
	m_node = new CylinderPrimitive(m_topRadius, m_bottomRadius, m_height);

	m_shape = new PhysicsShape();
	m_shape->initCylinderShapeZ(m_topRadius, m_bottomRadius, m_height);
	m_parent = nullptr;
	CannonPart::generateName();

	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	m_graphNode = new CannonPartNode(this);
	nodeEditor->addNode(m_graphNode);
	initAttachments();
}

void CannonPart::initAttachments()
{
	//forward backward
	m_attachment[0] = new Attachment(vec3(0.0, 0.0, m_height / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this);
}

Attachment * CannonPart::getAttachmentInfo(int index, vec3 & pos, vec3 & N, vec3 & up)
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

Attachment* CannonPart::getFirstAttachment()
{
	return m_attachment[0];
}

Attachment* CannonPart::getBottomAttachment()
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

Attachment* CannonPart::getAttachment(int index)
{
	return m_attachment[index];
}

int CannonPart::getAttachmentCount()
{
	return 1;
}

void CannonPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, u8"¼ÓÅ© %s",genShortName().c_str());
	setName(formatName);
}

int CannonPart::getType()
{
	return GAME_PART_CYLINDER;
}

void CannonPart::use()
{
	//shoot bullet
	float blockSize = 0.2;
	auto boxA = new CubePrimitive(blockSize, blockSize, blockSize);
	boxA->setPos(getWorldPos() + m_node->getForward() * (m_height / 2.0 + 0.01) + vec3(0, 0, 0));
	auto transform = boxA->getTransform();
	auto aabb = boxA->localAABB();
	auto rigA = PhysicsMgr::shared()->createRigidBody(1.0, transform, aabb);
	rigA->attach(boxA);
	rigA->setVelocity(m_node->getForward() * 20.0);
	g_GetCurrScene()->addNode(boxA);

	//apply recoil
	m_parent->m_rigid->applyImpulse(m_node->getForward() *-1* 50, m_node->getPos());
}
}
