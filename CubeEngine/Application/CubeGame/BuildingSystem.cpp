#include "BuildingSystem.h"
#include "3D/Primitive/CubePrimitive.h"
#include <algorithm>
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"

namespace tzw
{
TZW_SINGLETON_IMPL(BuildingSystem);
BuildingSystem::BuildingSystem(): m_node(nullptr)
{
}

void BuildingSystem::createNewToeHold(vec3 pos)
{
	//the toehold here
	m_node = new Drawable3D();
	m_node->setPos(pos);
	g_GetCurrScene()->addNode(m_node);
	auto part = new BlockPart();
	m_node->addChild(part->getNode());
	m_partList.insert(part);
}

void BuildingSystem::createPlaceByHit(vec3 pos, vec3 dir, float dist)
{
	std::vector<BlockPart *> tmp;
	for(auto iter:m_partList)
	{
		tmp.push_back(iter);
	}
	std::sort(tmp.begin(), tmp.end(), [&](BlockPart * left, BlockPart * right)
	{
		float distl = left->getNode()->getWorldPos().distance(pos);
		float distr = right->getNode()->getWorldPos().distance(pos);
		return distl < distr;
	}
	);
	for(auto iter:tmp)
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
		BlockPart * newPart = nullptr;
		if(isHit)
		{
			switch (side)
			{
			case RayAABBSide::up:
				{
					newPart = new BlockPart();
					newPart->getNode()->setPos(node->getPos() + up * 0.5);
				}
				break;
			case RayAABBSide::down:
				{
					newPart = new BlockPart();
					newPart->getNode()->setPos(node->getPos() + up * -0.5);
				}
				break;
			case RayAABBSide::left:
				{
					newPart = new BlockPart();
					newPart->getNode()->setPos(node->getPos() + right * -0.5);
				}
				break;
			case RayAABBSide::right:
				{
					newPart = new BlockPart();
					newPart->getNode()->setPos(node->getPos() + right * 0.5);
				}
				break;
			case RayAABBSide::front:
				{
					newPart = new BlockPart();
					newPart->getNode()->setPos(node->getPos() + forward * -0.5);
				}
				break;
			case RayAABBSide::back:
				{
					newPart = new BlockPart();
					newPart->getNode()->setPos(node->getPos() + forward * 0.5);
				}
				break;
			default:
				break;
			}
			if(newPart)
			{
				m_node->addChild(newPart->getNode());
				m_partList.insert(newPart);
				return;
			}
		}
	}
}

void BuildingSystem::cook()
{
	auto compundShape = new PhysicsCompoundShape();
	for(auto iter:m_partList)
	{
		auto mat = iter->getNode()->getLocalTransform();
		compundShape->addChildShape(&mat, iter->getShape()->getRawShape());
	}
	compundShape->finish();
	auto compundMat = m_node->getTransform();
	auto rig = PhysicsMgr::shared()->createRigidBodyFromCompund(1.0, &compundMat,compundShape);
	rig->attach(m_node);
}

Drawable3D* BuildingSystem::getNode()
{
	return m_node;
}
}
