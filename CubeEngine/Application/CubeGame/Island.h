#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>


namespace tzw
{
class PhysicsRigidBody;

class Island
{
public:
	std::set<BlockPart * > m_partList;
	Drawable3D * m_node;
	Island(vec3 pos);
	PhysicsRigidBody * m_rigid;
	void insert(BlockPart * part);
};


}
