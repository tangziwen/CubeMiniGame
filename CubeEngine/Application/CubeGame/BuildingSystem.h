#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
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
		void createNewToeHold(vec3 pos);
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
		void placeGamePart(GamePart * part, vec3 pos);
		void attachGamePartToConstraint(GamePart * part, Attachment * attach);
		void attachGamePartNormal(GamePart * part, Attachment * attach);
		void terrainForm(vec3 pos, vec3 dir, float dist, float value, float range);
		vec3 hitTerrain(vec3 pos, vec3 dir, float dist);
		void placeLiftPart(vec3 wherePos);
		void setCurrentControlPart(GamePart * controlPart);
		ControlPart * getCurrentControlPart() const;
		GamePart * createPart(int type);
		BearPart * placeBearingToAttach(Attachment * attach);
		SpringPart * placeSpringToAttach(Attachment * attach);
		Island * createIsland(vec3 pos);
		Attachment * rayTest(vec3 pos, vec3 dir, float dist);
		GamePart * rayTestPart(vec3 pos, vec3 dir, float dist);
		Island * rayTestIsland(vec3 pos, vec3 dir, float dist);
		LiftPart * getLift() const;
		void getIslandsByGroup(std::string islandGroup, std::vector<Island * > & groupList);
		void dump(std::string filePath);
		void load(std::string filePath);
		void updateBearing(float dt);
		void removeLiftConnected();
		void removeIsland(Island * island);
		std::set<GameConstraint *> &getConstraintList();
	private:
		ControlPart * m_controlPart;
		LiftPart * m_liftPart;
		unsigned int m_baseIndex;
		std::vector<Island *> m_IslandList;
		std::set<GameConstraint* > m_bearList;
		std::map<GamePart *, LabelNew *> m_partToLabel;
	};


}
