#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "GamePart.h"
#include <set>
#include <vector>
#include "rapidjson/document.h"
#include "Base/GuidObj.h"
namespace tzw
{
	class PhysicsCompoundShape;
	class PhysicsRigidBody;

class Island : public GuidObj
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
	void enablePhysics(bool isEnable);
	float getMass();
	void cook();
	void addNeighbor(Island * island);
	void removeNeighbor(Island * island);
	const std::set<Island*> getNeighBor() const;
	void dump(rapidjson::Value &island, rapidjson::Document::AllocatorType& allocator);
	void load(rapidjson::Value &island);
private:
	std::set<Island *> m_neighborIslands;
	PhysicsCompoundShape * m_compound_shape;
};


}
