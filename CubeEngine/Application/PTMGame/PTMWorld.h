#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "PTMMapCamera.h"
#include "PTMTile.h"
#include "PTMTown.h"
#include "PTMNation.h"
#define PTM_MAP_SIZE (96)
namespace tzw
{
	class PTMWorld : public Singleton<PTMWorld>
	{
	public:
		PTMWorld();
		void initMap();
		Node * getMapRootNode();
	private:
		PTMNation * createNation(std::string nationName);

		PTMTile * m_maptiles[PTM_MAP_SIZE][PTM_MAP_SIZE];
		std::vector<PTMTown * > m_pronviceList;
		std::vector<PTMNation * > m_nationList;

		PTMMapCamera * m_mapCamera;
		Node * m_mapRootNode;
		
	};

}