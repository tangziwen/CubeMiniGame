#include "PTMWorld.h"

#include <iostream>

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
namespace tzw
{


	PTMWorld::PTMWorld()
	{
	}

	void PTMWorld::initMap()
	{
		std::string filePath = "PTM/pronvices.bmp";
		auto data = Tfile::shared()->getData(filePath, true);
		int width;
		int height;
		int channels = 3;
		m_provincesBitMap = stbi_load_from_memory(data.getBytes(), data.getSize(), &width, &height, &channels, 3);
		std::cout<<width<<height<<channels;
		m_mapRootNode = Node::create();
		g_GetCurrScene()->addNode(m_mapRootNode);
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

		loadOwnerShips();

		auto window = GUIWindow::create("caonima", vec2(200, 350));
		g_GetCurrScene()->addNode(window);


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

		if (doc.HasMember("Nations"))
		{
			auto& nations = doc["Nations"];
			for (unsigned int i = 0; i < nations.Size(); i++)
			{
				auto& item = nations[i];
				auto nation = createNation(item["name"].GetString());
				nation->setIdx(item["id"].GetUint());
				m_nationIDMap[nation->getIdx()] = nation;
				m_nationNameMap[item["name"].GetString()] = nation;
				auto& flagColor = item["flag_color"];
				nation->setNationColor(vec3(flagColor[0].GetDouble(), flagColor[1].GetDouble(), flagColor[2].GetDouble()));
			}
		}
	}

	void PTMWorld::loadTowns()
	{
		rapidjson::Document doc;
		std::string filePath = "PTM/data/towns.json";
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

		if (doc.HasMember("Towns"))
		{
			auto& towns = doc["Towns"];
			for (unsigned int i = 0; i < towns.Size(); i++)
			{
				auto& item = towns[i];
				int x = item["pos"][0].GetInt();
				int y = PTM_MAP_SIZE - item["pos"][1].GetInt();
				PTMTile * tile = m_maptiles[x][y];
				auto town = new PTMTown(tile);
				town->setName(item["name"].GetString());
				m_pronviceList.push_back(town);
				m_townIDMap[item["id"].GetUint()] = town;

				//计算占领地块
				int r = item["own_color"][0].GetInt();
				int g = item["own_color"][1].GetInt();
				int b = item["own_color"][2].GetInt();

				for(int i = 0; i < PTM_MAP_SIZE; i++)
				{
					for(int j = 0; j < PTM_MAP_SIZE; j++)
					{
						int x0 = i;
						int y0 = j;//PTM_MAP_SIZE - j;//reverse
						bool checkR = m_provincesBitMap[ (y0 * PTM_MAP_SIZE + x0)*3 ] == r;
						bool checkG = m_provincesBitMap[ (y0 * PTM_MAP_SIZE + x0)*3 + 1] == g;
						bool checkB = m_provincesBitMap[ (y0 * PTM_MAP_SIZE + x0)*3 + 2] == b;
						if(checkR && checkG && checkB)
						{
							town->addOccupyTile(getTile(i, j));
						}
					}
					
				}
				std::string ownerName = item["owner"].GetString();
				m_nationNameMap[ownerName]->ownTown(town);
				town->updateGraphics();
				//auto army  = new PTMArmy(nullptr, tile);
				//army->updateGraphics();
			}
		}
	}

	void PTMWorld::loadOwnerShips()
	{
		return;
		rapidjson::Document doc;
		std::string filePath = "PTM/data/ownerships.json";
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

		if (doc.HasMember("Towns"))
		{
			auto& towns = doc["Towns"];
			for (unsigned int i = 0; i < towns.Size(); i++)
			{
				auto& item = towns[i];
				uint32_t townId = item["id"].GetUint();
				uint32_t nationID = item["owner"].GetUint();

				PTMTown * town = m_townIDMap[townId];
				m_nationIDMap[nationID]->ownTown(town);
				town->updateGraphics();
			}
		}
	}

}
