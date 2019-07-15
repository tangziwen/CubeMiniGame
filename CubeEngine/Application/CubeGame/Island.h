#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "GamePart.h"
#include <set>
#include <vector>


namespace tzw
{
	class PhysicsCompoundShape;
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
	PhysicsCompoundShape * getCompoundShape() const;
	void setCompoundShape(PhysicsCompoundShape * compoundShape);
	void recalculateCompound();
	float getMass();
	void cook();
private:
	PhysicsCompoundShape * m_compound_shape;
};


}
