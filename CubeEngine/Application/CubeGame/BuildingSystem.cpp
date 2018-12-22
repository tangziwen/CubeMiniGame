#include "BuildingSystem.h"
#include "3D/Primitive/CubePrimitive.h"
#include <algorithm>
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"

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

void BuildingSystem::placeNearBearing(Attachment * attach)
{
	auto iter = dynamic_cast<BlockPart * >(attach->m_parent);
	auto bearing = attach->m_bearPart;
	vec3 pos, n, up;
	attach->getAttachmentInfoWorld(pos, n, up);
	auto island = new Island(pos);
	m_IslandList.insert(island);
	auto part = new BlockPart();
	island->m_node->addChild(part->getNode());
	island->insert(part);
	bearing->m_a = part;
	bearing->m_b = iter;
	part->attachToFromOtherIsland(attach);
}

void BuildingSystem::createPlaceByHit(vec3 pos, vec3 dir, float dist)
{
	std::vector<BlockPart *> tmp;
	//search island
	for (auto island : m_IslandList)
	{
		
		for (auto iter : island->m_partList)
		{
			tmp.push_back(iter);
		}
	}
	std::sort(tmp.begin(), tmp.end(), [&](BlockPart * left, BlockPart * right)
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
			BlockPart * newPart = nullptr;
			if (isHit)
			{
				newPart = new BlockPart();
				/*if (iter->m_bearPart[int(side)]) // have some bear?
				{
					placeNearBearing(iter, node, side);
				}
				else*/
				{
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
						else // have some bear?
						{
							placeNearBearing(attach);
							//todo
						}
					}

				}
			return;
			}
		}
}

void BuildingSystem::createBearByHit(vec3 pos, vec3 dir, float dist)
{
	std::vector<BlockPart *> tmp;
	for (auto island : m_IslandList)
	{
		
		for (auto iter : island->m_partList)
		{
			tmp.push_back(iter);
		}
	}
	std::sort(tmp.begin(), tmp.end(), [&](BlockPart * left, BlockPart * right)
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
			bear->m_b = iter;
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
		compundShape->finish();
		auto compundMat = island->m_node->getTransform();
		auto rig = PhysicsMgr::shared()->createRigidBodyFromCompund(1.0, &compundMat,compundShape);
		rig->attach(island->m_node);
		island->m_rigid = rig;
	}

	for (auto bear : m_bearList)
	{
		auto partA = bear->m_a;
		auto partB = bear->m_b;
		vec3 pivotA, pivotB, axisA, asixB;
		findPiovtAndAxis(bear, partA, pivotA, axisA);
		findPiovtAndAxis(bear, partB, pivotB, asixB);
		PhysicsMgr::shared()->createHingeConstraint(partA->m_parent->m_rigid, partB->m_parent->m_rigid, pivotA, pivotB, axisA, asixB, false);
	}

}

//toDo
void BuildingSystem::findPiovtAndAxis(BearPart * bear, BlockPart * part, vec3 & pivot, vec3 & asix)
{
	auto sideA = 0;
	for(int i = 0; i< 6; i++)
	{
		if(part->m_bearPart[i] == bear)
		{
			sideA = i;
			break;
		}
	}
	auto node = part->getNode();
	auto m = node->getLocalTransform().data();
	vec3 up(m[4], m[5], m[6]);
	vec3 forward(-m[8], -m[9], -m[10]);
	vec3 right(m[0], m[1], m[2]);
	float offset = 0.25 + bearingGap / 2.0;
	switch (RayAABBSide(sideA))
	{
		{
		case RayAABBSide::up:
		{
			pivot = node->getPos() + up * offset;
			asix = up;
		}
		break;
		case RayAABBSide::down:
		{
			pivot = node->getPos() + up * -offset;
			asix = up;
		}
		break;
		case RayAABBSide::left:
		{
			pivot = node->getPos() + right * -offset;
			asix = right;
		}
		break;
		case RayAABBSide::right:
		{
			pivot = node->getPos() + right * offset;
			asix = right;
		}
		break;
		case RayAABBSide::front:
		{
			pivot = node->getPos() + forward * -offset;
			asix = forward;
		}
		break;
		case RayAABBSide::back:
		{
			pivot = node->getPos() + forward * offset;
			asix = forward;
		}
		break;
		default:
			break;
		}
	}
}

}
