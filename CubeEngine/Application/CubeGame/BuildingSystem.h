#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
namespace tzw
{

	class BuildingSystem
	{
		TZW_SINGLETON_DECL(BuildingSystem)
	public:
		BuildingSystem();
		void createNewToeHold(vec3 pos);
		void createPlaceByHit(vec3 pos, vec3 dir, float dist);
		void cook();
		Drawable3D * getNode();
	private:
		std::set<BlockPart * > m_partList;
		Drawable3D* m_node;
	};


}