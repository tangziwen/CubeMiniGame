#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "GamePart.h"
#include <set>
#include <vector>


namespace tzw
{
class PhysicsRigidBody;

class Island
{
public:
	std::vector<GamePart * > m_partList;
	Drawable3D * m_node;
	Island(vec3 pos);
	PhysicsRigidBody * m_rigid;
	void insert(GamePart * part);
	void remove(GamePart * part);
};


}
