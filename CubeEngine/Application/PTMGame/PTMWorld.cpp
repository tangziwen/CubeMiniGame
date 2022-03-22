#include "PTMWorld.h"

#include <iostream>

#include "PTMHero.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "Utility/file/Tfile.h"
#include "2D/GUIFrame.h"
#include "2D/GUITitledFrame.h"
#include "EngineSrc/Event/EventMgr.h"
#include "SOIL2/stb_image.h"
#include "2D/imgui.h"
#include "PTMInspectorGUI.h"
#include "PTMInGameEvent.h"
#include "2D/TileMap2DMgr.h"
#include "PTMConfig.h"
namespace tzw
{


	PTMWorld::PTMWorld()
	{
		;
	}

	void PTMWorld::initMap()
	{


		m_tileMgr = new TileMap2DMgr();
		m_tileMgr->initMap(PTM_MAP_SIZE, PTM_MAP_SIZE);

		//load Pop metaInfo

		PTMPopFactory::shared()->init();
		PTMHeroFactory::shared()->init();
		//g_GetCurrScene()->addNode(tileMgr);

		int tileTypeForest = m_tileMgr->addTileType("PTM/forest.png");
		int tileTypePlain = m_tileMgr->addTileType("PTM/plain.png");
		int tileTypeMountains = m_tileMgr->addTileType("PTM/mountains.png");
		int tileTypeWater = m_tileMgr->addTileType("PTM/water.png");

		std::string filePath = "PTM/pronvices.bmp";
		auto data = Tfile::shared()->getData(filePath, true);
		int width;
		int height;
		int channels = 3;
		m_provincesBitMap = stbi_load_from_memory(data.getBytes(), data.getSize(), &width, &height, &channels, 3);
		std::cout<<width<<height<<channels;
		m_mapRootNode = Node::create();
		g_GetCurrScene()->addNode(m_mapRootNode);

		m_mapRootNode->addChild(m_tileMgr);
		m_hud = new PTMHUD();
		m_hud->init();

		m_controller = new PTMPlayerController();

		m_mapCamera = new PTMMapCamera(m_mapRootNode);

		loadNations();
		m_controller->control(m_nationList[0]);//test control the first country
		m_hud->setController(m_controller);
		for(int i = 0; i < PTM_MAP_SIZE; i++)
		{
			for(int j = 0; j < PTM_MAP_SIZE; j++)
			{
				auto tile = new PTMTile();
				tile->coord_x = i;
				tile->coord_y = j;
				m_maptiles[i][j] = tile;
				int r = m_provincesBitMap[ (j * PTM_MAP_SIZE + i )* 3];
				int g = m_provincesBitMap[ (j * PTM_MAP_SIZE + i )* 3 + 1];
				int b = m_provincesBitMap[ (j * PTM_MAP_SIZE + i )* 3 + 2];
				if(r ==0 && g == 0 && b == 255)
				{
					tile->m_tileType = PTMTileType::TILE_OCEAN;
				}
				tile->m_graphics->updateGraphics();
			}
		}

		loadTowns();

		for(PTMNation * nation : m_nationList)
		{
			nation->initData();
		}
		loadOwnerShips();

		EventMgr::shared()->addFixedPiorityListener(this);

		PTMGameTimeMgr::shared()->setStartDate(207,11,1);
		PTMGameTimeMgr::shared()->setOnDayTickCallback(
			[this](uint32_t day)
			{
				dailyTick();
			}
		);

		PTMGameTimeMgr::shared()->setOnMonthTickCallback(
			[this]()
			{
				monthlyTick();
			}
		);
		PTMGameTimeMgr::shared()->setOnPauseStateChangedCallback(
			[this](bool isPause)
			{
				m_hud->updateTimePauseState(isPause);
				//dailyTick();
			}
		);
		m_hud->updateAll();
		PTMInspectorGUI::shared()->init();

		PTMInGameEventMgr::shared()->loadEventsFromFile("PTM/data/Event/default.json");
		return;

	}

	Node* PTMWorld::getMapRootNode()
	{
		return m_mapRootNode;
	}

	void PTMWorld::onFrameUpdate(float dt)
	{
		PTMGameTimeMgr::shared()->tick(dt);
	}

	bool PTMWorld::onKeyPress(int keyCode)
	{
		switch(keyCode)
		{
		case TZW_KEY_SPACE:
			PTMGameTimeMgr::shared()->togglePause();
			m_hud->updateTimePauseState(PTMGameTimeMgr::shared()->isPause());
			break;
		default:
			break;
		};
		return false;
	}

	PTMTile* PTMWorld::getTile(int x, int y)
	{
		if(x < 0 || x >= PTM_MAP_SIZE)
		{
			return nullptr;
		}
		if(y < 0 || y >= PTM_MAP_SIZE)
		{
			return nullptr;
		}
		return m_maptiles[x][y];
	}

	TileMap2DMgr* PTMWorld::getTileMgr()
	{
		return m_tileMgr;
	}

	PTMTown* PTMWorld::getTown(int x, int y)
	{
		if(x >= 0 && x < townWidth && y>=0 && y< townWidth)
		{
			return m_mapTowns[x][y];
		}
		else
		{
			return nullptr;
		}
		
	}

	PTMNation* PTMWorld::createNation(std::string nationName)
	{
		auto nation = new PTMNation();
		nation->setName(nationName);
		m_nationList.push_back(nation);
		return nation;
	}

	void PTMWorld::dailyTick()
	{
		//update hud
		m_hud->updateTimeOfDay();
		for(auto nation : m_nationList)
		{
			nation->onDailyTick();
		}
	}

	void PTMWorld::monthlyTick()
	{
		for(PTMNation * nation :m_nationList)
		{
			nation->onMonthlyTick();
		}
		m_hud->updateMonthly();
	}

	void PTMWorld::simulateAttack()
	{

	}

	void PTMWorld::loadNations()
	{
		rapidjson::Document doc;
		std::string filePath = "PTM/data/nations.json";
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}
		auto& re = TbaseMath::getRandomEngine();
		if (doc.HasMember("Nations"))
		{
			auto& nations = doc["Nations"];
			//rank should be mini[0] 30% small[1] 40% medium[2] 20% huge[3] 10%
			
			int totalNationSize = nations.Size();
			std::vector<int> rankCount ={ std::max((int)(std::ceil(0.3f * totalNationSize)), 1), 
				std::max((int)(std::ceil( 0.4f * totalNationSize)), 1), std::max((int)(std::ceil(0.2f * totalNationSize)), 1), 
				std::max((int)(std::ceil(0.1f * totalNationSize)), 1)};
			for (unsigned int i = 0; i < nations.Size(); i++)
			{
				auto& item = nations[i];
				auto nation = createNation(item["name"].GetString());
				nation->setIdx(item["id"].GetUint());
				m_nationIDMap[nation->getIdx()] = nation;
				m_nationNameMap[item["name"].GetString()] = nation;
				auto& flagColor = item["flag_color"];
				nation->setNationColor(vec3(flagColor[0].GetDouble(), flagColor[1].GetDouble(), flagColor[2].GetDouble()));
				NationLevel level;
				level.m_nation = nation;
				int rankIdx = 0;
				do{
					 rankIdx = re() % rankCount.size();

				} while (rankCount[rankIdx] <= 0);
				rankCount[rankIdx] -= 1;
				switch(rankIdx)
				{
				case 0:
					level.count = 0 + re()%2;
				break;
				case 1:
					level.count = 2 + re()%3;
				break;
				case 2:
					level.count = 4 + re()%4;
				break;
				case 3:
					level.count = 6 + re()%5;
				break;
				}
				m_nationLevelMap.push_back(level);
			}
		}
	}

	void PTMWorld::loadTowns()
	{
		int townGrid = 6;
		int townWidth = PTM_MAP_SIZE / townGrid;
		int id = 0;
		for(int i = 0; i < townWidth; i++)
		{
			for(int j = 0; j < townWidth; j++)
			{
				int x = i * townGrid + rand() % (townGrid - 1);
				int y = j * townGrid + rand() % (townGrid - 1);

				int r = m_provincesBitMap[ (y * PTM_MAP_SIZE + x )* 3];
				int g = m_provincesBitMap[ (y * PTM_MAP_SIZE + x )* 3 + 1];
				int b = m_provincesBitMap[ (y * PTM_MAP_SIZE + x )* 3 + 2];
				if(r ==0 && g == 0 && b == 255)
				{
					m_mapTowns[i][j] = nullptr;
					continue;
				}
				PTMTile * tile = m_maptiles[x][y];
				auto town = new PTMTown(tile);
				m_mapTowns[i][j] = town;
				town->town_x = i;
				town->town_y = j;
				town->setName("test");
				m_pronviceList.push_back(town);
				m_townIDMap[id] = town;
				town->initPops();
				town->updateGraphics();
				id ++;
			}
		}
	}

	void PTMWorld::loadOwnerShips()
	{
		auto& re = TbaseMath::getRandomEngine();
		for(PTMNation *nation : m_nationList)
		{
			PTMTown * capital;
			do
			{
				int rndIndx = re() % m_pronviceList.size();
				capital = m_pronviceList[rndIndx];
			}while(capital->getOwner());//choose the capital
			nation->ownTown(capital);
		}
		
		//iterate to conquer

		for(auto & nationLevel : m_nationLevelMap)
		{
			PTMNation * nation = nationLevel.m_nation;
			auto & townList = nation->getTownList();

			for(int i = 0; i < nationLevel.count; i++)
			{
				PTMTown * rndTown = townList[re()%townList.size()];
				int x = rndTown->town_x, y = rndTown->town_y;
				std::vector<PTMTown *> tmpList;
				PTMTown * tNeighbor = nullptr;
				tNeighbor = getTown(x - 1, y -1);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x + 1, y +1);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x + 1, y -1);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x + 1, y -1);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x + 1, y);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x - 1, y);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x, y + 1);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				tNeighbor = getTown(x, y - 1);
				if(tNeighbor && tNeighbor->getOwner() == nullptr)
				{
					tmpList.push_back(tNeighbor);
				}
				if(!tmpList.empty())
				{
					auto conqueredTown = tmpList[re()%tmpList.size()];
					nation->ownTown(conqueredTown);
				}
			}
		}

		
	}

}
