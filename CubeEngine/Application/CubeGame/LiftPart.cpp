#include "LiftPart.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include <algorithm>
#include "BuildingSystem.h"
#include "3D/Primitive/CylinderPrimitive.h"

namespace tzw
{
const float blockSize = 0.5;
LiftPart::LiftPart()
{
	m_liftHeight = 0.0;
	m_node = new Drawable3D();
	m_plaftormPart = new CubePrimitive(blockSize, blockSize, blockSize);
	auto texture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_basecolor.png");
	m_plaftormPart->getMaterial()->setTex("DiffuseMap", texture);
	float pipeLength = 25.0;
	m_pipePart = new CylinderPrimitive(0.10f, 0.10f, pipeLength);

	m_basePart = new CubePrimitive(1.5, 1.5, 0.3);
	m_plaftormPart->addChild(m_pipePart);
	m_pipePart->setRotateE(vec3(90, 0, 0));
	m_pipePart->setPos(0, -pipeLength / 2.0f, 0.0);
	m_node->addChild(m_plaftormPart);
	m_node->addChild(m_basePart);
	m_shape = new PhysicsShape();
	m_shape->initBoxShape(vec3(blockSize, blockSize, blockSize));
	m_parent = nullptr;
	m_effectedIslandGroup = nullptr;
	initAttachments();
	g_GetCurrScene()->addNode(m_node);
}

LiftPart::~LiftPart()
{
	//m_node->removeFromParent();
}

void LiftPart::highLight()
{
	if(m_node)
	{
		m_plaftormPart->setColor(vec4(1.0, 0.5, 0.5, 1.0));
		m_basePart->setColor(vec4(1.0, 0.5, 0.5, 1.0));
		m_pipePart->setColor(vec4(1.0, 0.5, 0.5, 1.0));
	}
}

void LiftPart::unhighLight()
{
	if(m_node)
	{
		m_plaftormPart->setColor(vec4(1.0, 1.0, 1.0, 1.0));
		m_basePart->setColor(vec4(1.0, 1.0, 1.0, 1.0));
		m_pipePart->setColor(vec4(1.0, 1.0, 1.0, 1.0));
	}
}

void LiftPart::setIsVisible(bool isVisible)
{
	m_plaftormPart->setIsVisible(isVisible);
	m_basePart->setIsVisible(isVisible);
	m_pipePart->setIsVisible(isVisible);
}

void LiftPart::setMaterial(Material* mat)
{
	m_plaftormPart->setMaterial(mat);
	m_basePart->setMaterial(mat);
	m_pipePart->setMaterial(mat);
}

Attachment * LiftPart::findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & attachUp)
{
	vec3 hitPoint;
	Attachment * attachPtr = nullptr;

	//only top area can used to place
	if(m_attachment[4]->isHit(ray, hitPoint))
	{
		attachPtr = getAttachmentInfo(4, attachPosition, Normal, attachUp);
	}
	return attachPtr;
}

void LiftPart::initAttachments()
{
	//forward backward
	m_attachment[0] = new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this);
	m_attachment[1] = new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this);

	//right left
	m_attachment[2] = new Attachment(vec3(blockSize / 2.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this);
	m_attachment[3] = new Attachment(vec3(-blockSize / 2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this);

	//up down
	m_attachment[4] = new Attachment(vec3(0.0, blockSize / 2.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this);
	m_attachment[4]->m_locale = "up";
	m_attachment[5] = new Attachment(vec3(0.0, -blockSize / 2.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this);
}

Attachment * LiftPart::getAttachmentInfo(int index, vec3 & pos, vec3 & N, vec3 & up)
{
	auto mat = m_plaftormPart->getLocalTransform();
	auto atta = m_attachment[index];
	vec4 a_pos = vec4(atta->m_pos, 1.0);
	vec4 a_n = vec4(atta->m_normal, 0.0);
	vec4 a_up = vec4(atta->m_up, 0.0);
	pos = mat.transofrmVec4(a_pos).toVec3();
	N = mat.transofrmVec4(a_n).toVec3();
	up = mat.transofrmVec4(a_up).toVec3();
	return m_attachment[index];
}

Attachment* LiftPart::getFirstAttachment()
{
	return m_attachment[4];
}

void LiftPart::liftUp(float val)
{
	if (m_effectedIslandGroup) 
	{
		m_liftHeight += val;
		m_liftHeight = std::min(m_liftHeight, 10.0f);

		std::vector<Island *> groupList;
		auto&islandList = m_effectedIslandGroup->getIslandList();
        for (auto island : islandList) 
		{
			auto oldPos = island->m_node->getPos();
			oldPos.y += val;
			island->m_node->setPos(oldPos);
        }
		vec3 oldPos = m_plaftormPart->getPos();
		oldPos.y += val;
		m_plaftormPart->setPos(oldPos);
	}
}

void LiftPart::setEffectedIsland(Vehicle* islandGroup)
{
	m_effectedIslandGroup = islandGroup;
}

void LiftPart::setPos(vec3 v)
{
	m_node->setPos(v);
}

bool LiftPart::isHit(Ray ray)
{
	bool isHitPlatform = m_plaftormPart->isHit(ray);
	bool isHitBase = m_basePart->isHit(ray);
	bool isHitCylinder = m_pipePart->isHit(ray);
	return isHitBase || isHitPlatform || isHitCylinder;
}

Drawable3D* LiftPart::getNode() const
{
	return m_plaftormPart;
}

GamePartType LiftPart::getType()
{
	return GamePartType::GAME_PART_LIFT;
}
}
