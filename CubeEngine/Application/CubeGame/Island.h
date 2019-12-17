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
	~Island();
	PhysicsRigidBody * m_rigid;
	void insertNoUpdatePhysics(GamePart * part);
	void insert(GamePart*part);
	void insertAndAdjustAttach(GamePart * part, Attachment * attach, int attachIndex);
	void remove(GamePart * part);
	void removeAll();
	PhysicsCompoundShape * getCompoundShape() const;
	void setCompoundShape(PhysicsCompoundShape * compoundShape);
	void recalculateCompound();
	void enablePhysics(bool isEnable);
	bool isEnablePhysics();
	float getMass();
	void cook();
	void addNeighbor(Island * island);
	void removeNeighbor(Island * island);
	const std::set<Island*> getNeighBor() const;
	void dump(rapidjson::Value &island, rapidjson::Document::AllocatorType& allocator);
	void load(rapidjson::Value &island);
	bool m_isSpecial;
	std::string m_islandGroup;
	void genIslandGroup();
	void killAllParts();
	void updatePhysics();
	void recordBuildingRotate();
	void recoverFromBuildingRotate();
	void updateNeighborConstraintPhysics();
	void loadInternalConnected();
	AABB getAABBInWorld();
private:
	bool m_enablePhysics;
	//the building rotation, island need to record the rotation(while the vehicle is in the lift part) too!!!! because if we don't do that, the next we load or replace to the Lift from ground,
	//we can not determine which attachment is the correct center-bottom one to dock at the lift.
	Quaternion m_buildingRotate;
	std::set<Island *> m_neighborIslands;
	PhysicsCompoundShape * m_compound_shape;
};


}
