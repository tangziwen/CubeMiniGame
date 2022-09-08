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
	loadConfigImpl("RL/PlayerCharacter.json", true);
	loadConfigImpl("RL/Monsters.json", false);
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

const std::vector<RLHeroData>& RLHeroCollection::getPlayableHeroDatas()
{
	return m_playableHeroDataCollection;
}

void RLHeroCollection::getHeroRangeFromTier(std::string typeName, int tier, std::vector<RLHeroData*>& dataList, bool ignoreMob)
{
	for(RLHeroData& data : m_MonsterHeroDataCollection)
	{
		if(data.m_tier == tier)
		{
			bool passTypeCheck = true;
			if(typeName != "Any")
			{
				passTypeCheck = (data.m_type == typeName);
			}
			if(passTypeCheck)
			{
				if(ignoreMob)
				{
					if(!data.m_isMob)
						dataList.push_back(&data);
				}
				else
				{
					dataList.push_back(&data);
				}
			}
		}
	}
}

void RLHeroCollection::getMobsFromTiers(int tier, std::vector<RLHeroData*>& dataList)
{
	for(RLHeroData& data : m_MonsterHeroDataCollection)
	{
		if(data.m_tier == tier && data.m_isMob)
		{
			dataList.push_back(&data);
				
		}
	}
}

void RLHeroCollection::loadConfigImpl(std::string filePath, bool isPlayable)
{
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
		data.m_speed = node["Speed"].GetFloat();
		data.m_sprite = node["Sprite"].GetString();
		data.m_aiType = node["AI_Type"].GetString();
		if(node.HasMember("Difficulty"))
		{
			data.m_difficulty = node["Difficulty"].GetFloat();
		}
		if(node.HasMember("Tier"))
		{
			data.m_tier = node["Tier"].GetInt();
		}
		if(node.HasMember("DefaultWeapon"))
		{
			data.m_defaultWeapon  = node["DefaultWeapon"].GetString();
		}else
		{
			data.m_defaultWeapon.clear();
		}
		if(node.HasMember("IsMob"))
		{
			data.m_isMob  = node["IsMob"].GetBool();
		}else
		{
			data.m_isMob = false;
		}
		if(node.HasMember("Type"))
		{
			data.m_type  = node["Type"].GetString();
		}else
		{
			data.m_type = "";
		}
		data.m_id = m_heroDataCollection.size();
		m_heroDataCollection.push_back(data);
		if(isPlayable)
		{
			m_playableHeroDataCollection.push_back(data);
		}
		else
		{
			m_MonsterHeroDataCollection.push_back(data);
		}
	}
}

}
