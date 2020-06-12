#pragma once
#include "Engine/EngineDef.h"
#include "GameConstraint.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include "LiftPart.h"
#include "ControlPart.h"
#include "SpringPart.h"

namespace tzw
{
class LabelNew;
class PhysicsHingeConstraint;

class BuildingSystem :public Singleton<BuildingSystem>
{
public:
	BuildingSystem();
	void flipBearingByHit(vec3 pos, vec3 dir, float dist);
	void flipBearing(GamePart * gamePart);
	void placeItem(GameItem * item, vec3 pos, vec3 dir, float dist);
	void dropFromLift();
	void replaceToLiftByRay(vec3 pos, vec3 dir, float dist);
	void replaceToLift(Island * island, Attachment * attachment, LiftPart * targetLift);
	void tempPlace(Island * island, LiftPart * targetLift);
	Attachment* replaceToLiftIslands(Vehicle * vehicle);
	void replaceToLift_R(Island * island, std::set<Island *> & closeList, LiftPart * liftPart);
	void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);
	void tmpMoveWheel(bool isOpen);
	void removePartByAttach(Attachment * attach);
	void removePart(GamePart * attach);
	void placeGamePartStatic(GamePart * part, vec3 pos);
	void attachGamePartToConstraint(GamePart * part, Attachment * attach, float degree);
	void attachGamePart(GamePart * part, Attachment * attach, float degree, int index);
	void terrainForm(vec3 pos, vec3 dir, float dist, float value, float range);
	void terrainPaint(vec3 pos, vec3 dir, float dist, int matIndex, float range);
	vec3 hitTerrain(vec3 pos, vec3 dir, float dist);
	void placeLiftPart(vec3 wherePos);
	void setCurrentControlPart(GamePart * controlPart);
	int getGamePartTypeInt(GamePart *);
	ControlPart * getCurrentControlPart() const;
	GamePart * createPart(int type, std::string itemName);
	BearPart * placeBearingToAttach(Attachment * attach, std::string itemName);
	SpringPart * placeSpringToAttach(Attachment * attach, std::string itemName);
	Attachment * rayTest(vec3 pos, vec3 dir, float dist);
	GamePart * rayTestPart(vec3 pos, vec3 dir, float dist);
	GamePart * rayTestPartAny(vec3 pos, vec3 dir, float dist);
	GamePart * rayTestPartXRay(vec3 pos, vec3 dir, float dist);
	Island * rayTestIsland(vec3 pos, vec3 dir, float dist);
	LiftPart * getLift() const;
	void addThruster(GamePart * thruster);
	void removeThruster(GamePart * thrsuter);
	void liftStore(GamePart * part);
	void getIslandsByGroup(std::string islandGroup, std::vector<Island * > & groupList);
	void dumpVehicle(std::string filePath);
	void loadVehicle(std::string filePath);
	void clearStatic();
	void loadStatic(rapidjson::Value &island);
	void dumpStatic(rapidjson::Value &island, rapidjson::Document::AllocatorType& allocator);
	void updateBearing(float dt);
	void removeLiftConnected();
	void removeAll();
	void removeByGroup(std::string islandGroup);
	void removeIsland(Island * island);
	void update(float dt);
	std::set<Vehicle * >& getVehicleList();
private:
	bool m_isInXRayMode;
public:
	bool isIsInXRayMode() const;
	void setIsInXRayMode(const bool isInXRayMode);
private:
	Vehicle * m_storeIslandGroup;
public:
	Vehicle* getStoreIslandGroup() const;
	void setStoreIslandGroup(Vehicle* storeIslandGroup);
private:
	ControlPart * m_controlPart;
	LiftPart * m_liftPart;
	unsigned int m_baseIndex;
	std::vector<Island *> m_staticIsland;
	std::set<GamePart* > m_thrusterList;
	std::map<GamePart *, LabelNew *> m_partToLabel;
	GamePart * m_currPointPart;
	std::set<Vehicle * > m_vehicleList;
	Vehicle * m_staticVehicle;
};


}
