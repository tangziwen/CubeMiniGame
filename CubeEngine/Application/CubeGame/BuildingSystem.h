#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"

namespace tzw
{

	class BuildingSystem
	{
		TZW_SINGLETON_DECL(BuildingSystem)
	public:
		BuildingSystem();
		void createNewToeHold(vec3 pos);
		void placeNearBearing(Attachment * attach);
		void createPlaceByHit(vec3 pos, vec3 dir, float dist);
		void createBearByHit(vec3 pos, vec3 dir, float dist);
		void cook();
		void findPiovtAndAxis(BearPart * bear, BlockPart * part, vec3 & pivot, vec3 & asix);
	private:
		std::set<Island *> m_IslandList;
		std::set<BearPart* > m_bearList;
	};


}
