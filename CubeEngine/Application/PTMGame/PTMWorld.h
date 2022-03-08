#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "PTMMapCamera.h"
#include "PTMTile.h"
#include "PTMTown.h"
#include "PTMNation.h"
#include "PTMPlayerController.h"
#include "PTMHUD.h"
#include "PTMGameTimeMgr.h"
#include "PTMArmy.h"
#include "PTMTownGUI.h"

#define PTM_MAP_SIZE (96)
namespace tzw
{
	class TileMap2DMgr;
	class PTMWorld : public Singleton<PTMWorld>, public EventListener
	{
	public:
		PTMWorld();
		void initMap();
		Node * getMapRootNode();
		void onFrameUpdate(float dt) override;
		bool onKeyPress(int keyCode) override;
		PTMPlayerController * getPlayerController() {return m_controller;};
		PTMTile * getTile(int x, int y);
		TileMap2DMgr * getTileMgr();
	private:
		PTMNation * createNation(std::string nationName);

		PTMTile * m_maptiles[PTM_MAP_SIZE][PTM_MAP_SIZE];
		std::vector<PTMTown * > m_pronviceList;
		std::vector<PTMNation * > m_nationList;
		std::unordered_map<uint32_t, PTMNation * > m_nationIDMap;
		std::unordered_map<std::string, PTMNation * > m_nationNameMap;
		std::unordered_map<uint32_t, PTMTown * > m_townIDMap;

		//time
		void dailyTick();
		void monthlyTick();
		void simulateAttack();
		PTMMapCamera * m_mapCamera = nullptr;
		Node * m_mapRootNode = nullptr;
		PTMHUD * m_hud = nullptr;
		PTMPlayerController * m_controller = nullptr;
		PTMTownGUI * m_townGUI = nullptr;
		PTMTown * m_currInspectTown;
		unsigned char * m_provincesBitMap = nullptr;
		TileMap2DMgr * m_tileMgr;

		void loadNations();
		void loadTowns();
		void loadOwnerShips();

		
	};

}