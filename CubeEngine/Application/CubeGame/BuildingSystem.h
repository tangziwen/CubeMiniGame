#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include "LiftPart.h"
namespace tzw
{
class PhysicsHingeConstraint;

class BuildingSystem
	{
		TZW_SINGLETON_DECL(BuildingSystem)
	public:
		BuildingSystem();
		void createNewToeHold(vec3 pos);
		void placePartNextToBearing(Attachment * attach, int type);
		void placePartByHit(vec3 pos, vec3 dir, float dist, int type);
		void placeBearingByHit(vec3 pos, vec3 dir, float dist);
		void flipBearingByHit(vec3 pos, vec3 dir, float dist);
		void placeItem(GameItem * item, vec3 pos, vec3 dir, float dist);
		void cook();
		void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);
		void tmpMoveWheel();
		void removePartByHit(vec3 pos, vec3 dir, float dist);
		void placeGamePart(GamePart * part, vec3 pos);
		void attachGamePartToBearing(GamePart * part, Attachment * attach);
		void attachGamePartNormal(GamePart * part, Attachment * attach);
		vec3 hitTerrain(vec3 pos, vec3 dir, float dist);
		void placeLiftPart(vec3 wherePos);
		GamePart * createPart(int type);
		BearPart * placeBearingToAttach(Attachment * attach);
		Island * createIsland(vec3 pos);
		Attachment * rayTest(vec3 pos, vec3 dir, float dist);
	private:
		LiftPart * m_liftPart;
		std::set<Island *> m_IslandList;
		std::set<BearPart* > m_bearList;
		std::list<PhysicsHingeConstraint *> m_constrainList;
	};


}
