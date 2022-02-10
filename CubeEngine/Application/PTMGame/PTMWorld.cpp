#include "PTMWorld.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "Utility/file/Tfile.h"
namespace tzw
{


	PTMWorld::PTMWorld()
	{
	}

	void PTMWorld::initMap()
	{
		m_mapRootNode = Node::create();
		g_GetCurrScene()->addNode(m_mapRootNode);
		m_mapCamera = new PTMMapCamera(m_mapRootNode);

		loadNations();
		auto wei = createNation("Wei");
		wei->setNationColor(vec3(0.1, 0.1, 0.8));
		auto shu = createNation("Shu");
		shu->setNationColor(vec3(0.1, 0.8, 0.1));
		auto wu = createNation("Wu");
		wu->setNationColor(vec3(0.8, 0.1, 0.8));

		for(int i = 0; i < PTM_MAP_SIZE; i++)
		{
			for(int j = 0; j < PTM_MAP_SIZE; j++)
			{
				auto tile = new PTMTile();
				tile->coord_x = i;
				tile->coord_y = j;
				m_maptiles[i][j] = tile;
				tile->m_graphics->updateGraphics();
			}
		}

		loadTowns();

		loadOwnerShips();
	}

	Node* PTMWorld::getMapRootNode()
	{
		return m_mapRootNode;
	}

	PTMNation* PTMWorld::createNation(std::string nationName)
	{
		auto nation = new PTMNation();
		nation->setName(nationName);
		m_nationList.push_back(nation);
		return nation;
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
				int y = item["pos"][1].GetInt();
				PTMTile * tile = m_maptiles[x][y];
				auto town = new PTMTown(tile);
				town->setName(item["name"].GetString());
				m_pronviceList.push_back(town);
				m_townIDMap[item["id"].GetUint()] = town;
				town->updateGraphics();
			}
		}
	}

	void PTMWorld::loadOwnerShips()
	{
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