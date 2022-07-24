#include "RLHeroCollection.h"

#include <cstdlib>

#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw
{
RLHeroCollection::RLHeroCollection()
{
}
void RLHeroCollection::loadConfig()
{
	std::string filePath = "RL/Monsters.json";
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d",
			filePath.c_str(),
			doc.GetParseError(),
			doc.GetErrorOffset());
		exit(1);
	}
	for(int i = 0; i < doc.Size(); i++)
	{
		auto& node = doc[i];
		RLHeroData data;
		data.m_name = node["Name"].GetString();
		data.m_maxHealth = node["MaxHealth"].GetFloat();
		data.m_meleeDamage = node["MeleeDamage"].GetFloat();
		data.m_sprite = node["Sprite"].GetString();
		m_heroDataCollection.push_back(data);
	}
}

RLHeroData* RLHeroCollection::getHeroData(int id)
{

	return &m_heroDataCollection[id];
}

RLHeroData* RLHeroCollection::getHeroData(std::string name)
{
	return getHeroData(getHeroIDByName(name));
}

int RLHeroCollection::getHeroIDByName(std::string name)
{
	for(size_t i = 0; i < m_heroDataCollection.size(); i++)
	{
		RLHeroData & data = m_heroDataCollection[i];
		if(data.m_name == name)
		{
			return i;
		}
	}
	return -1;
}

}
