#include "Island.h"
#include "../../EngineSrc/Scene/SceneMgr.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "Collision/PhysicsRigidBody.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "BlockPart.h"
#include "BuildingSystem.h"
#include "CylinderPart.h"
#include "Base/GuidMgr.h"
#include "ThrusterPart.h"
#include "CannonPart.h"
#include <algorithm>

#include "AudioSystem/AudioSystem.h"
#include "ButtonPart.h"
#include "SwitchPart.h"
#include "PlanterPart.h"
namespace tzw {
Vehicle* Island::getVehicle() const
{
	return m_vehicle;
}

void Island::setVehicle(Vehicle* const vehicle)
{
	m_vehicle = vehicle;
	if(vehicle)
	{
		vehicle->addIsland(this);
	}
}

Island::Island(vec3 pos, Vehicle * vehicle)
{
	// the toehold here
	m_node = new Drawable3D();
	m_node->setIsAccpectOcTtree(false);
	m_node->setPos(pos);
	g_GetCurrScene()->addNode(m_node);
	m_rigid = nullptr;
	m_compound_shape = nullptr;
	m_isSpecial = false;
	m_buildingRotate = Quaternion();
	m_enablePhysics = false;
	m_isStatic = false;
	setVehicle(vehicle);
}

Island::~Island()
{
	//SAYONARA, fellas
	for(auto neighbor : m_neighborIslands)
	{
		neighbor->removeNeighbor(this);
	}
	if(m_rigid && m_rigid->isInWorld())
	{
		PhysicsMgr::shared()->removeRigidBody(m_rigid);
	}
	delete m_rigid;
}

void
Island::insertNoUpdatePhysics(GamePart* part)
{
	if (!part->getNode()) {
	tlog("insert \n");
	}
	m_partList.push_back(part);
	part->m_parent = this;
	m_node->addChild(part->getNode());
	part->setVehicle(m_vehicle);
	if(part->isConstraint())
	{
		m_vehicle->addConstraint(static_cast<GameConstraint *>(part));
	}
	
}

void Island::insert(GamePart* part)
{
	insertNoUpdatePhysics(part);
	//���������ԣ�������Ľ���Ҳ���趼��ס
	std::vector<Attachment * > attachmentList;
	for(auto anyPart :m_partList)
	{
		if(part != anyPart) 
		{
			for(int i = 0; i< anyPart->getAttachmentCount(); i++)
			{
				auto attach = anyPart->getAttachment(i);
				if(!attach->m_connected) 
				{
					attachmentList.push_back(attach);
				}
			}
		}
	}
	int count = 0;
	for(int i = 0; i < part->getAttachmentCount(); i++)
	{
		auto selfAttach = part->getAttachment(i);
		if(!selfAttach->m_connected)
		{
			for(auto attach : attachmentList)
			{
				if(!attach->m_connected)
				{
					vec3 p1,n1,u1;
					selfAttach->getAttachmentInfo(p1, n1, u1);
					vec3 p2,n2,u2;
					attach->getAttachmentInfo(p2, n2, u2);
					if(p1.distance(p2) < 0.001)
					{
						selfAttach->m_connected = attach;
						attach->m_connected = selfAttach;
						count += 1;
					}
				}
			}
		}
	}
	tlog("extra connected Num %d", count);
	updatePhysics();
	updateNeighborConstraintPhysics();
}

void Island::insertAndAdjustAttach(GamePart * part, Attachment * attach, int attachIndex)
{
	insertNoUpdatePhysics(part);
	part->attachToOtherIslandByAlterSelfPart(attach, attachIndex);
	//we need update self physics rigidbody and connected constraint
	updatePhysics();
	updateNeighborConstraintPhysics();
}

void Island::remove(GamePart* part)
{
	auto result = std::find(m_partList.begin(), m_partList.end(), part);
	if (result != m_partList.end()) {
	m_partList.erase(result);
	}
	part->m_parent = this;
	//break the connect
	for(int i =0; i< part->getAttachmentCount(); i++)
	{
		auto attach = part->getAttachment(i);
		//we need break the constraint
		if(attach->m_connected)
		{
			if(attach->m_connected->m_parent->isConstraint())
			{
				auto constraint = static_cast<GameConstraint *> (attach->m_connected->m_parent);
				if(constraint->m_a == attach)
				{
					constraint->m_a = nullptr;
				}
				if(constraint->m_b == attach)
				{
					constraint->m_b = nullptr;
				}
				for(int i = 0; i < constraint->getAttachmentCount(); i++)
				{
					auto constraintAttach = constraint->getAttachment(i);
					if(constraintAttach->m_connected == attach)
					{
						constraintAttach->m_connected = nullptr;
					}
				}
				if(constraint->m_a == nullptr && constraint->m_b == nullptr)
				{
					//the bearing is not connected any thing. let it die.
					BuildingSystem::shared()->removePart(constraint);
				} else
				{
					constraint->updateConstraintState();
				}
			}
		attach->breakConnection();
		}
	}
	//we need update self rigid body physics related
	if(!m_partList.empty())
	{
		updatePhysics();
	}
	updateNeighborConstraintPhysics();
}

void Island::removeAll()
{
	m_partList.clear();
}

PhysicsCompoundShape*
Island::getCompoundShape() const
{
  return m_compound_shape;
}

void
Island::setCompoundShape(PhysicsCompoundShape* compoundShape)
{
  m_compound_shape = compoundShape;
}

void
Island::recalculateCompound()
{
	if (!getCompoundShape())
	return;
	if(m_partList.empty())
	return;
	if(m_isSpecial)
	{
		printf("aaa");
		return;
	}
	for (auto part : m_partList) {
	if (part->getType() != GamePartType::GAME_PART_LIFT) {
	  auto mat = part->getNode()->getLocalTransform();
	  m_compound_shape->addChildShape(&mat, part->getShape()->getRawShape());
	} else 
	{
		
	}
	}

	//!!!ATTENTION
	//After adjustPrincipalAxis
	//All parts' LocalMat = inverse(principleMat) * localMat
	//The island's islandMat = islandMat * principleMat
	AABB localAABB;
	auto principleMat = m_compound_shape->adjustPrincipalAxis([this](int index) { return m_partList[index]->getMass(); });
	for (auto part : m_partList) 
	{
		if (part->getType() != GamePartType::GAME_PART_LIFT) 
		{
		  auto mat = part->getNode()->getLocalTransform();
		  mat = principleMat.inverted() * mat;
		  part->getNode()->setPos(mat.getTranslation());
		  Quaternion q;
		  q.fromRotationMatrix(&mat);
		  part->getNode()->setRotateQ(q);
		}
	}
	m_compound_shape->calculateLocalInertia(getMass());
	auto mat = m_node->getLocalTransform();
	mat =  mat * principleMat;
	m_node->setPos(mat.getTranslation());
	Quaternion q;
	q.fromRotationMatrix(&mat);
	m_node->setRotateQ(q);
	m_node->reCache();
	m_node->setLocalAABB(localAABB);
}

void
Island::enablePhysics(bool isEnable)
{
	if (m_enablePhysics == isEnable) return;
	m_enablePhysics = isEnable;
	if(m_isSpecial)
	{
		//auto constraint = static_cast<GameConstraint*>(m_partList[0]);
		//constraint->enablePhysics(isEnable);
	}
	else
	{
		if(isEnable) 
		{
			auto mat = m_node->getTransform();
			m_rigid->setWorldTransform(mat);
			PhysicsMgr::shared()->addRigidBody(m_rigid);
			//û��activate�ǲ�����Ч�ģ���Ϊ���������Ѿ�sleep�ˣ����Ի�����
			m_rigid->activate();
		} else 
		{
	        if(m_rigid) 
			{
				PhysicsMgr::shared()->removeRigidBody(m_rigid);
				m_rigid->clearAll();
	        }
		}
	}


}

	bool Island::isEnablePhysics()
	{
		return m_enablePhysics;
	}

	float Island::getMass()
	{
		if(m_isStatic)
			return 0.0f;
	  float mass = 0.0f;
	  for (auto part : m_partList) {
	    if (part->getType() != GamePartType::GAME_PART_LIFT) {
	      mass += part->getMass();
	    }
	  }
	  return mass;
	}

void
Island::cook()
{
	PhysicsCompoundShape* compoundShape = getCompoundShape();
	if (!compoundShape) {
	compoundShape = new PhysicsCompoundShape();
	setCompoundShape(compoundShape);
	}
	recalculateCompound();
	auto partMat = m_node->getTransform();
	float theMass = getMass();
	auto rig = PhysicsMgr::shared()->createRigidBodyFromCompund(
	theMass, &partMat, compoundShape);
	rig->attach(m_node);
	m_rigid = rig;
	m_rigid->m_onHitCallBack = std::bind(&Island::onHitCallBack, this,std::placeholders::_1);
}

void
Island::addNeighbor(Island* island)
{
  m_neighborIslands.insert(island);
}

void
Island::removeNeighbor(Island* island)
{
  auto result = m_neighborIslands.find(island);
  if (result != m_neighborIslands.end()) {
    m_neighborIslands.erase(result);
  }
}

const std::set<Island*>
Island::getNeighBor() const
{
  return m_neighborIslands;
}

void Island::dump(rapidjson::Value &island, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value partList(rapidjson::kArrayType);
	island.AddMember("UID", std::string(getGUID()), allocator);
	for(auto i : m_partList)
	{
		rapidjson::Value partDocObj(rapidjson::kObjectType);
		i->dump(partDocObj, allocator);
		partList.PushBack(partDocObj, allocator);
	}
	island.AddMember("name", 100.0, allocator);
	island.AddMember("partList", partList, allocator);

	
	//rotation, island need to record the rotation(where the vehicle is in the lift part) too!!!! because if we don't do that, the next we load ,
	//we can not determine which attachment is the correct center-bottom one to dock at the lift.
	auto rotate = m_buildingRotate;
	rapidjson::Value rotateList(rapidjson::kArrayType);
	rotateList.PushBack(rotate.x, allocator).PushBack(rotate.y, allocator).PushBack(rotate.z, allocator).PushBack(rotate.w, allocator);

	island.AddMember("rotate", rotateList, allocator);
	
	rapidjson::Value posList(rapidjson::kArrayType);
	auto pos = m_node->getPos();
	posList.PushBack(pos.x, allocator).PushBack(pos.y, allocator).PushBack(pos.z, allocator);
	island.AddMember("pos", posList, allocator);
}

void Island::load(rapidjson::Value& island)
{
	setGUID(island["UID"].GetString());

	//rotate
	if(island.HasMember("rotate"))
	{
		auto q = Quaternion(island["rotate"][0].GetDouble(),island["rotate"][1].GetDouble(), island["rotate"][2].GetDouble(), island["rotate"][3].GetDouble());
		m_node->setRotateQ(q);
		m_buildingRotate = q;
	}

	if(island.HasMember("pos"))
	{
		auto p = vec3(island["pos"][0].GetDouble(),island["pos"][1].GetDouble(), island["pos"][2].GetDouble());
		m_node->setPos(p);
	}
	if (island.HasMember("partList"))
	{
		auto& partList = island["partList"];
		for (unsigned int i = 0; i < partList.Size(); i++)
		{
			auto& item = partList[i];
			GamePart * part = nullptr;
			//��ȡ��ʱ����ΪIsland�ڵĲ����Ѿ��洢��λ����Ϣ�������insert��Ȼupdate������Ϣ�Ļ����ᵼ��Node�ľ���仯�����û᲻��ȷ
			switch((GamePartType)item["type"].GetInt())
			{
				case GamePartType::GAME_PART_BLOCK:
				{
					part = new BlockPart(item["ItemName"].GetString());
                }
				break;

				case GamePartType::GAME_PART_CYLINDER:
					{
						part = new CylinderPart();
					}
				break;
				case GamePartType::GAME_PART_CONTROL:
					{
						part = new ControlPart(item["ItemName"].GetString());
					}
				break;
				case GamePartType::GAME_PART_CANNON:
					{
						part = new CannonPart(item["ItemName"].GetString());
					}
				break;
				case GamePartType::GAME_PART_THRUSTER:
					{
						part = new ThrusterPart(item["ItemName"].GetString());
					}
				break;
				case GamePartType::GAME_PART_BUTTON:
					{
						part = new ButtonPart(item["ItemName"].GetString());
					}
				break;
				case GamePartType::GAME_PART_PLANTER:
					{
						part = new PlanterPart(item["ItemName"].GetString());
					}
				break;
				case GamePartType::GAME_PART_SWITCH:
					{
						part = new SwitchPart(item["ItemName"].GetString());
					}
				break;
			}
			if(part)
			{
				part->setVehicle(m_vehicle);
				part->load(item);
				insertNoUpdatePhysics(part);
			}
		}

		//ͳһUpdateһ������
		updatePhysics();
		updateNeighborConstraintPhysics();

		//����һ���ڲ���connected
		loadInternalConnected();
	}
}

std::string Island::getIslandGroup() const
{
	return m_islandGroup;
}

void Island::setIslandGroup(const std::string& islandGroup)
{
	m_islandGroup = islandGroup;
}

void Island::genIslandGroup()
{
	m_islandGroup = GUIDMgr::shared()->genGUID();
}

void Island::killAllParts()
{

	
}

void Island::updatePhysics()
{
	if(!m_rigid)
	{
		cook();
	}
	if(m_enablePhysics)
	{
		PhysicsMgr::shared()->removeRigidBody(m_rigid);
	}
	if (getCompoundShape()) 
	{
		//delete old compound shape, there may be a better performance method.
		delete m_compound_shape;
	    setCompoundShape(new PhysicsCompoundShape());
		recalculateCompound();
		m_rigid->setMass(getMass(), getCompoundShape()->calculateLocalInertia(getMass()));
		m_rigid->updateInertiaTensor();
		// recalculate will modify the transform position, though the final
		// islandMatrix * childMatrix will remain the same, we need to recalculate
		// the rigid-body position
		auto startTransform = m_node->getTransform();
		m_rigid->setWorldTransform(startTransform);
		tlog("update compound!");
	}
	m_rigid->setCollisionShape(getCompoundShape());
	m_rigid->updateInertiaTensor();
	btTransform ident;
	ident.setIdentity();
	btVector3 aabbMin,aabbMax;
	getCompoundShape()->getRawShape()->getAabb(ident,aabbMin,aabbMax);
	float radius = std::max(std::max(aabbMax.getX() - aabbMin.getX(), aabbMax.getY() - aabbMin.getY()),aabbMax.getZ() - aabbMin.getZ());
	m_rigid->setCcdSweptSphereRadius(0.00001);
	m_rigid->setCcdMotionThreshold(0.5);
	if(m_enablePhysics)
	{
		PhysicsMgr::shared()->addRigidBody(m_rigid);
		//û��activate�ǲ�����Ч�ģ���Ϊ���������Ѿ�sleep�ˣ����Ի�����
		m_rigid->activate();
	}
}

void Island::recordBuildingRotate()
{
	m_buildingRotate = m_node->getRotateQ();
}

void Island::recoverFromBuildingRotate()
{
	m_node->setRotateQ(m_buildingRotate);
}

void Island::updateNeighborConstraintPhysics()
{
	for(auto island :m_neighborIslands)
	{
		if(island->m_isSpecial)
		{
			auto constraint = static_cast<GameConstraint *>(island->m_partList[0]);
			constraint->updateConstraintState();
		}

	}
}

void Island::loadInternalConnected()
{
	//handle all internal connection
	for(auto part : m_partList)
	{
		for(int i = 0; i < part->getAttachmentCount(); i++)
		{
			auto  attach = part->getAttachment(i);
			if(!attach->m_connectedGUID.empty())
			{
				auto constraintAttach_target = reinterpret_cast<Attachment*>(
					GUIDMgr::shared()->get(attach->m_connectedGUID));
				//internal attaches
				if(constraintAttach_target && constraintAttach_target->m_parent->m_parent == attach->m_parent->m_parent)
				{
					attach->m_connected = constraintAttach_target;
					constraintAttach_target->m_connected = attach;
				}
			}
		}
	}
}

AABB Island::getAABBInWorld()
{
	if(m_rigid->isInWorld())
	{
		return m_rigid->getAABBInWorld();
	}else
	{
	btVector3 aabbMin,aabbMax;
	btTransform worldTrans;
	worldTrans.setIdentity();
	worldTrans.setFromOpenGLMatrix(this->m_node->getTransform().data());
	getCompoundShape()->getRawShape()->getAabb(worldTrans,aabbMin,aabbMax);
	AABB result;
	result.update(vec3(aabbMin.x(),aabbMin.y(), aabbMin.z()));
	result.update(vec3(aabbMax.x(),aabbMax.y(), aabbMax.z()));
	return result;
	}
}

void Island::onHitCallBack(vec3 p)
{
	AudioSystem::shared()->playOneShotSound(AudioSystem::DefaultOneShotSound::ITEM_DROP);
}

bool Island::isIsStatic() const
{
	return m_isStatic;
}

void Island::setIsStatic(const bool isStatic)
{
	m_isStatic = isStatic;
	if(isStatic && !m_enablePhysics)
	{
		updatePhysics();
		enablePhysics(true);
	}
}
}
