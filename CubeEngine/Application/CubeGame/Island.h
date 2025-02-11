#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "GamePart.h"
#include <set>
#include <vector>
#include "rapidjson/document.h"
#include "Base/GuidObj.h"
#include "Vehicle.h"
namespace tzw
{
	class PhysicsCompoundShape;
	class PhysicsRigidBody;

class Island : public GuidObj
{
public:
	Vehicle* getVehicle() const;
	void setVehicle(Vehicle* const vehicle);
	std::vector<GamePart * > m_partList;
	Drawable3D * m_node;
	Island(vec3 pos, Vehicle * vehicle);
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
	
	std::string getIslandGroup() const;
	void setIslandGroup(const std::string& islandGroup);
	void genIslandGroup();
	void killAllParts();
	void updatePhysics();
	void recordBuildingRotate();
	void recoverFromBuildingRotate();
	void updateNeighborConstraintPhysics();
	void loadInternalConnected();
	AABB getAABBInWorld();
	void onHitCallBack(vec3 p);
public:
	bool isIsStatic() const;
	void setIsStatic(const bool isStatic);
private:
	bool m_isStatic;
	std::string m_islandGroup;
	bool m_enablePhysics;
	//the building rotation, island need to record the rotation(while the vehicle is in the lift part) too!!!! because if we don't do that, the next we load or replace to the Lift from ground,
	//we can not determine which attachment is the correct center-bottom one to dock at the lift.
	Quaternion m_buildingRotate;
	std::set<Island *> m_neighborIslands;
	PhysicsCompoundShape * m_compound_shape;
	Vehicle * m_vehicle;
};


}
