#include "s_GameSystem.h"
#include <stdio.h>
#include "External/TUtility/TUtility.h"
#include "Engine/Engine.h"

namespace tzw
{
	TZW_SINGLETON_IMPL(s_GameSystem);

	void s_GameSystem::start(std::string scenario)
	{
		printf("the game start!\n");
		loadCities();
		

	}

	void s_GameSystem::loadCities()
	{
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(Engine::shared()->getUserPath("Strategy/cities.json"), true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		auto& attributes = doc["cities"];
		for(unsigned int i = 0; i < attributes.Size(); i++)
		{
			auto& attribute = attributes[i];
			auto city = new s_City();
			city->setName(attribute["name"].GetString());
			city->setLevel(attribute["level"].GetInt());
			city->setPopulation(attribute["population"].GetInt());
			city->setPos(ivec2(attribute["pos"][0].GetInt(),attribute["pos"][1].GetInt()));
			m_cityList.push_back(city);
		}
	}

	void s_GameSystem::loadScenario(std::string scenario)
	{
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(Engine::shared()->getUserPath(scenario), true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			printf("get json data err! %d offset %d\n",doc.GetParseError(), doc.GetErrorOffset());
			return;
		}
		auto& attributes = doc["faction"];
		for(unsigned int i = 0; i < attributes.Size(); i++)
		{
			auto& attribute = attributes[i];
			auto faction = new s_Faction();
			faction->setName(attribute["name"].GetString());
			faction->setGold(attribute["gold"].GetInt());
			faction->setMilitaryLevel(attribute["millitaryLevel"].GetInt());
			faction->setPoliticalLevel(attribute["politicalLevel"].GetInt());
			auto & ownList = attribute[i];
			m_factionList.push_back(faction);

		}
	}

	s_GameSystem::s_GameSystem()
	{

	}

}