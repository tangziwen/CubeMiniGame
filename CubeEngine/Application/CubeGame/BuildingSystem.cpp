#include "BuildingSystem.h"
#include "3D/Primitive/CubePrimitive.h"
#include <algorithm>
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "CylinderPart.h"

namespace tzw
{
const float bearingGap = 0.00;
TZW_SINGLETON_IMPL(BuildingSystem);
BuildingSystem::BuildingSystem()
{
}

void BuildingSystem::createNewToeHold(vec3 pos)
{
	auto newIsland = new Island(pos);
	m_IslandList.insert(newIsland);
	auto part = new BlockPart();
	newIsland->m_node->addChild(part->getNode());
	newIsland->insert(part);
}

void BuildingSystem::placePartNearBearing(Attachment * attach, int type)
{
	auto iter = dynamic_cast<BlockPart * >(attach->m_parent);
	auto bearing = attach->m_bearPart;
	vec3 pos, n, up;
	attach->getAttachmentInfoWorld(pos, n, up);
	auto island = new Island(pos + n * 0.5);
	m_IslandList.insert(island);
	GamePart * part = nullptr;
	switch (type)
	{
	case 0:
		part = new BlockPart();
		break;
	case 1:
		part = new CylinderPart();
		break;
	}
	island->m_node->addChild(part->getNode());
	island->insert(part);
	part->attachToFromOtherIsland(attach, bearing);
}

void BuildingSystem::placePartByHit(vec3 pos, vec3 dir, float dist, int type)
{
	std::vector<GamePart *> tmp;
	//search island
	for (auto island : m_IslandList)
	{
		
		for (auto iter : island->m_partList)
		{
			tmp.push_back(iter);
		}
	}
	std::sort(tmp.begin(), tmp.end(), [&](GamePart * left, GamePart * right)
	{
		float distl = left->getNode()->getWorldPos().distance(pos);
		float distr = right->getNode()->getWorldPos().distance(pos);
		return distl < distr;
	}
	);
		for (auto iter : tmp)
		{
			auto island = iter->m_parent;
			auto node = iter->getNode();
			auto invertedMat = node->getTransform().inverted();
			vec4 dirInLocal = invertedMat * vec4(dir, 0.0);
			vec4 originInLocal = invertedMat * vec4(pos, 1.0);

			auto r = Ray(originInLocal.toVec3(), dirInLocal.toVec3());
			RayAABBSide side;
			vec3 hitPoint;
			auto isHit = r.intersectAABB(node->localAABB(), &side, hitPoint);
			auto m = node->getLocalTransform().data();
			vec3 up(m[4], m[5], m[6]);
			vec3 forward(-m[8], -m[9], -m[10]);
			vec3 right(m[0], m[1], m[2]);
			up.normalize();
			forward.normalize();
			right.normalize();
			GamePart * newPart = nullptr;
			if (isHit)
			{
				if(type == 0)
				{
					newPart = new BlockPart();
				}
				else if (type == 1)
				{
					newPart = new CylinderPart();
				}
				vec3 attachPos, attachNormal, attachUp;
				auto attach = iter->findProperAttachPoint(r, attachPos, attachNormal,attachUp);
				if(attach)
				{
					if(!attach->m_bearPart)
					{
						newPart->attachTo(attach);
						if (newPart)
						{
							island->m_node->addChild(newPart->getNode());
							island->insert(newPart);
						}
					}
					else // have some bearing?
					{
						placePartNearBearing(attach, type);
						//todo
					}
				}
			return;
			}
		}
}

void BuildingSystem::placeBearByHit(vec3 pos, vec3 dir, float dist)
{
	std::vector<GamePart *> tmp;
	for (auto island : m_IslandList)
	{
		
		for (auto iter : island->m_partList)
		{
			tmp.push_back(iter);
		}
	}
	std::sort(tmp.begin(), tmp.end(), [&](GamePart * left, GamePart * right)
	{
		float distl = left->getNode()->getWorldPos().distance(pos);
		float distr = right->getNode()->getWorldPos().distance(pos);
		return distl < distr;
	}
	);
	for (auto iter : tmp)
	{
		auto node = iter->getNode();
		auto invertedMat = node->getTransform().inverted();
		vec4 dirInLocal = invertedMat * vec4(dir, 0.0);
		vec4 originInLocal = invertedMat * vec4(pos, 1.0);

		auto r = Ray(originInLocal.toVec3(), dirInLocal.toVec3());
		RayAABBSide side;
		vec3 hitPoint;
		auto isHit = r.intersectAABB(node->localAABB(), &side, hitPoint);
		auto m = node->getLocalTransform().data();
		vec3 up(m[4], m[5], m[6]);
		vec3 forward(-m[8], -m[9], -m[10]);
		vec3 right(m[0], m[1], m[2]);
		up.normalize();
		forward.normalize();
		right.normalize();
		vec3 a,b,c;
		auto attachment = iter->findProperAttachPoint(r, a, b, c);
		if (attachment)
		{
			//connect a BearPart
			auto bear = new BearPart();
			bear->m_b = attachment;
			attachment->m_bearPart = bear;
			m_bearList.insert(bear);
			break;
		}
	}
}

void BuildingSystem::cook()
{
	//each island, we create a rigid
	for (auto island : m_IslandList)
	{
		auto compundShape = new PhysicsCompoundShape();
		for(auto iter:island->m_partList)
		{
			auto mat = iter->getNode()->getLocalTransform();
			compundShape->addChildShape(&mat, iter->getShape()->getRawShape());
		}
		//compundShape->finish();
		auto compundMat = island->m_node->getTransform();
		auto rig = PhysicsMgr::shared()->createRigidBodyFromCompund(1.0, &compundMat,compundShape);
		
		rig->attach(island->m_node);
		island->m_rigid = rig;
	}

	for (auto bear : m_bearList)
	{
		auto attachA = bear->m_a;
		auto attachB = bear->m_b;
		auto partA = static_cast<BlockPart *>(attachA->m_parent);
		auto partB = static_cast<BlockPart *>(attachB->m_parent);

		vec3 worldPosA, worldNormalA, worldUpA;
		attachA->getAttachmentInfoWorld(worldPosA, worldNormalA, worldUpA);
		vec3 worldPosB, worldNormalB, worldUpB;
		attachB->getAttachmentInfoWorld(worldPosB, worldNormalB, worldUpB);
		vec3 hingeDir = (worldPosB - worldPosA).normalized();
		vec3 pivotA, pivotB, axisA, axisB;
		findPiovtAndAxis(attachA, hingeDir, pivotA, axisA);
		findPiovtAndAxis(attachB, hingeDir, pivotB, axisB);

		//Õâ¸öhinge tmdÕæ¿Ó
		PhysicsMgr::shared()->createHingeConstraint(partA->m_parent->m_rigid, partB->m_parent->m_rigid, pivotA, pivotB, axisA, axisB, false);
	}
}

//toDo
void BuildingSystem::findPiovtAndAxis(Attachment * attach, vec3 hingeDir,  vec3 & pivot, vec3 & asix)
{
	auto part = static_cast<BlockPart * >(attach->m_parent);
	auto island = part->m_parent;
	auto islandInvertedMatrix = island->m_node->getLocalTransform().inverted();
	
	auto transform = part->getNode()->getLocalTransform();
	auto normalInIsland = transform.transofrmVec4(vec4(attach->m_normal, 0.0)).toVec3();

	pivot = transform.transofrmVec4(vec4(attach->m_pos, 1.0)).toVec3();
	asix = islandInvertedMatrix.transofrmVec4(vec4(hingeDir, 0.0)).toVec3();
}

}
