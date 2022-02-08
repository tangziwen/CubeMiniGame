#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "PTMMapCamera.h"
#include "PTMTile.h"
#include "PTMTown.h"
#define PTM_MAP_SIZE (4)
namespace tzw
{
	class PTMWorld : public Singleton<PTMWorld>
	{
	public:
		PTMWorld();
		void initMap();
		Node * getMapRootNode();
	private:
		PTMTile * m_maptiles[PTM_MAP_SIZE][PTM_MAP_SIZE];
		std::vector<PTMTown * > m_pronviceList;

		PTMMapCamera * m_mapCamera;
		Node * m_mapRootNode;
		
	};

}