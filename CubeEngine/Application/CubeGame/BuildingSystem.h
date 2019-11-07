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

class BuildingSystem
{
	TZW_SINGLETON_DECL(BuildingSystem)
public:
	BuildingSystem();
	void flipBearingByHit(vec3 pos, vec3 dir, float dist);
	void placeItem(GameItem * item, vec3 pos, vec3 dir, float dist);
	void dropFromLift();
	void replaceToLiftByRay(vec3 pos, vec3 dir, float dist);
	void replaceToLift(Island * island, Attachment * attachment = nullptr);
	void tempPlace(Island * island);
	Attachment* replaceToLiftIslands(std::string islandGroup);
	void replaceToLift_R(Island * island, std::set<Island *> & closeList);
	void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);
	void tmpMoveWheel(bool isOpen);
	void removePartByAttach(Attachment * attach);
	void removePart(GamePart * attach);
	void placeGamePart(GamePart * part, vec3 pos);
	void attachGamePartToConstraint(GamePart * part, Attachment * attach, float degree);
	void attachGamePart(GamePart * part, Attachment * attach, float degree, int index);
	void terrainForm(vec3 pos, vec3 dir, float dist, float value, float range);
	vec3 hitTerrain(vec3 pos, vec3 dir, float dist);
	void placeLiftPart(vec3 wherePos);
	void setCurrentControlPart(GamePart * controlPart);
	int getGamePartTypeInt(GamePart *);
	ControlPart * getCurrentControlPart() const;
	GamePart * createPart(int type, std::string itemName);
	BearPart * placeBearingToAttach(Attachment * attach, std::string itemName);
	SpringPart * placeSpringToAttach(Attachment * attach);
	Island * createIsland(vec3 pos);
	Attachment * rayTest(vec3 pos, vec3 dir, float dist);
	GamePart * rayTestPart(vec3 pos, vec3 dir, float dist);
	GamePart * rayTestPartXRay(vec3 pos, vec3 dir, float dist);
	Island * rayTestIsland(vec3 pos, vec3 dir, float dist);
	LiftPart * getLift() const;
	void addThruster(GamePart * thruster);
	void removeThruster(GamePart * thrsuter);

	void getIslandsByGroup(std::string islandGroup, std::vector<Island * > & groupList);
	void dump(std::string filePath);
	void load(std::string filePath);
	void updateBearing(float dt);
	void removeLiftConnected();
	void removeAll();
	void removeIsland(Island * island);
	std::set<GameConstraint *> &getConstraintList();
	void update(float dt);
private:
	bool m_isInXRayMode;
public:
	bool isIsInXRayMode() const;
	void setIsInXRayMode(const bool isInXRayMode);
private:
	ControlPart * m_controlPart;
	LiftPart * m_liftPart;
	unsigned int m_baseIndex;
	std::vector<Island *> m_IslandList;
	std::set<GameConstraint* > m_bearList;
	std::set<GamePart* > m_thrusterList;
	std::map<GamePart *, LabelNew *> m_partToLabel;
	GamePart * m_currPointPart;
};


}
