#include "CannonPart.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "NodeEditorNodes/CannonPartNode.h"
#include "GameUISystem.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleInitVelocityModule.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletMgr.h"


namespace tzw
{
static auto blockSize = 0.25;
CannonPart::CannonPart():m_firingVelocity(50.0f),m_recoil(50.0f)
{

	m_topRadius = 0.1;
	m_bottomRadius = 0.1;
	m_height = 0.8;
	m_node = new CylinderPrimitive(m_topRadius, m_bottomRadius, m_height);
	
	auto block = new CubePrimitive(blockSize, blockSize, blockSize);
	block->setPos(0, 0, m_height/ 2 - blockSize/ 2.0);
	m_node->addChild(block);

	m_shape = new PhysicsShape();
	m_shape->initCylinderShapeZ(m_topRadius, m_bottomRadius, m_height);
	m_parent = nullptr;
	CannonPart::generateName();

	initAttachments();
}

CannonPart::CannonPart(std::string itemName):m_firingVelocity(50.0f),m_recoil(50.0f)
{
	m_topRadius = 0.1;
	m_bottomRadius = 0.1;
	m_height = 0.8;
	GamePart::initFromItemName(itemName);
	m_parent = nullptr;
	CannonPart::generateName();

	auto nodeEditor = m_vehicle->getEditor();
	m_graphNode = new CannonPartNode(this);
	nodeEditor->addNode(m_graphNode);
}

CannonPart::~CannonPart()
{
	auto nodeEditor = m_vehicle->getEditor();
	nodeEditor->removeNode(m_graphNode);
	delete m_graphNode;
}

void CannonPart::initAttachments()
{
	//forward backward
	addAttachment(new Attachment(vec3(0.0, 0.0, m_height / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this));

	//right left
	addAttachment(new Attachment(vec3(blockSize / 2.0, 0.0, m_height / 2.0 - blockSize / 2.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this));
	addAttachment(new Attachment(vec3(-blockSize / 2.0, 0.0, m_height / 2.0 - blockSize / 2.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this));

	//up down
	addAttachment(new Attachment(vec3(0.0, blockSize / 2.0, m_height / 2.0 - blockSize / 2.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this));
	addAttachment(new Attachment(vec3(0.0, -blockSize / 2.0, m_height / 2.0 - blockSize / 2.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this));
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
	sprintf_s(formatName, 512, TRC(u8"加农 %s"),genShortName().c_str());
	setName(formatName);
}

GamePartType CannonPart::getType()
{
	return GamePartType::GAME_PART_CANNON;
}

void CannonPart::drawInspect()
{
	drawInspectNameEdit();
	float force = getFiringVelocity();
	if(ImGui::InputFloat(TRC(u8"投射物初速度"), &force))
	{
		setFiringVelocity(force);
	}
	float recoil = getRecoil();
	if(ImGui::InputFloat(TRC(u8"后坐力"), &recoil))
	{
		setRecoil(recoil);
	}
	if(ImGui::RadioButton(TRC(u8"抛射物"),m_bulletMode == 0)) m_bulletMode = 0;
	if(ImGui::RadioButton(TRC(u8"激光"),m_bulletMode == 1)) m_bulletMode = 1;
	if(ImGui::RadioButton(TRC(u8"脉冲激光"),m_bulletMode == 2)) m_bulletMode = 2;
	
}

void CannonPart::use()
{
	//shoot bullet
	auto firePos = getWorldPos() - m_node->getForward() * (m_height / 2.0 + 0.01) + vec3(0, 0, 0);
	switch (m_bulletMode)
	{
		case 0:
			BulletMgr::shared()->fire(firePos,  m_node->getForward() * -1, getFiringVelocity(), BulletType::Projecttile);
		break;
		case 1:
			BulletMgr::shared()->fire(firePos,  m_node->getForward() * -1, getFiringVelocity(), BulletType::HitScanLaser);
		break;
		case 2:
			BulletMgr::shared()->fire(firePos,  m_node->getForward() * -1, getFiringVelocity(), BulletType::PulseLaser);
		break;
	}
	


	tlog("fire");
	auto emitter = new ParticleEmitter(1);
	emitter->setIsLocalPos(true);
	emitter->setTex("Texture/flare.bmp");
	emitter->setSpawnRate(0.3);
	emitter->addInitModule(new ParticleInitSizeModule(1.1, 1.3));
	emitter->addInitModule(new ParticleInitVelocityModule(vec3(0, 0.0, 0.0), vec3(0, 0.0, 0.0 )));
	emitter->addInitModule(new ParticleInitLifeSpanModule(0.3, 0.3));
	emitter->addInitModule(new ParticleInitAlphaModule(0.6, 0.6));
	emitter->addUpdateModule(new ParticleUpdateColorModule(vec4(1.0, 1.0, 0.0, 1.0), vec4(0.6, 0.6, 0.0, 0.1)));
	emitter->setIsState(ParticleEmitter::State::Playing);
	emitter->setDepthBias(0.05);
	emitter->setPos(vec3(0.0, 0.0, 0.4));
	emitter->setIsInfinite(false);


	auto emitter2 = new ParticleEmitter(1);
	emitter2->setIsLocalPos(true);
	emitter2->setTex("ParticleTex/smoke_04.png");
	emitter2->setSpawnRate(0.3);
	emitter2->addInitModule(new ParticleInitSizeModule(2.0, 2.1));
	emitter2->addInitModule(new ParticleInitVelocityModule(vec3(0, 0.0, 0.0), vec3(0, 0.0, 0.0 )));
	emitter2->addInitModule(new ParticleInitLifeSpanModule(0.3, 0.3));
	emitter2->addInitModule(new ParticleInitAlphaModule(0.6, 0.6));
	emitter2->addUpdateModule(new ParticleUpdateColorModule(vec4(1.0, 1.0, 0.3, 1.0), vec4(0.26, 0.26, 0.0, 0.1)));
	emitter2->setIsState(ParticleEmitter::State::Playing);
	emitter2->setPos(vec3(0.0, 0.0, 0.4));
	emitter2->setDepthBias(0.05);
	emitter2->setIsInfinite(false);
	m_node->addChild(emitter);
	m_node->addChild(emitter2);
	
	//apply recoil
	m_parent->m_rigid->applyImpulse(m_node->getForward() * getRecoil(), m_node->getPos());
}

bool CannonPart::isNeedDrawInspect()
{
	return true;
}

GraphNode* CannonPart::getGraphNode() const
{
	return m_graphNode;
}

void CannonPart::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
	GamePart::dump(partData, allocator);
	partData.AddMember("FiringVelocity", m_firingVelocity, allocator);
	partData.AddMember("Recoil", m_recoil, allocator);
	partData.AddMember("BulletMode", m_bulletMode, allocator);
}

void CannonPart::load(rapidjson::Value& partData)
{
	GamePart::load(partData);
	if(partData.HasMember("FiringVelocity"))
		m_firingVelocity = partData["FiringVelocity"].GetDouble();
	if(partData.HasMember("Recoil"))
		m_recoil = partData["Recoil"].GetDouble();
	if(partData.HasMember("BulletMode"))
		m_bulletMode = partData["BulletMode"].GetDouble();
}

void CannonPart::AddOnVehicle(Vehicle* vehicle)
{
	m_graphNode = new CannonPartNode(this);
	vehicle->getEditor()->addNode(m_graphNode);
}

float CannonPart::getFiringVelocity() const
{
	return m_firingVelocity;
}

void CannonPart::setFiringVelocity(const float firingVelocity)
{
	m_firingVelocity = firingVelocity;
}

float CannonPart::getRecoil() const
{
	return m_recoil;
}

void CannonPart::setRecoil(const float recoil)
{
	m_recoil = recoil;
}
}
