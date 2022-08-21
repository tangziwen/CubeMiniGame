#include "RLWeaponCollection.h"

#include <cstdlib>

#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw
{
RLWeaponCollection::RLWeaponCollection()
{
}
void RLWeaponCollection::loadConfig()
{
	std::string filePath = "RL/Weapons.json";
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
		RLWeaponData data;
		data.m_name = node["Name"].GetString();
		data.m_damage = node["Damage"].GetFloat();
		data.m_cd = node["CD"].GetFloat();
		data.m_bulletSpeed = node["BulletSpeed"].GetFloat();
		data.m_fireRate = node["FireRate"].GetFloat();
		data.m_fireRound = node["FireRound"].GetInt();
		if (node.HasMember("SplitNum"))
		{
			data.m_splitNum = node["SplitNum"].GetInt();
		}
		else{
			data.m_splitNum = 0;
		}
		if (node.HasMember("SplitAngle"))
		{
			data.m_splitAngle = node["SplitAngle"].GetFloat();
		}
		else{
			data.m_splitAngle = 45.0f;
		}

		data.m_id = i;
		m_weaponDataCollection.push_back(data);
	}
}

RLWeaponData* RLWeaponCollection::getWeaponData(int id)
{

	return &m_weaponDataCollection[id];
}

RLWeaponData* RLWeaponCollection::getWeaponData(std::string name)
{
	return getWeaponData(getWeaponIDByName(name));
}

int RLWeaponCollection::getWeaponIDByName(std::string name)
{
	for(size_t i = 0; i < m_weaponDataCollection.size(); i++)
	{
		RLWeaponData & data = m_weaponDataCollection[i];
		if(data.m_name == name)
		{
			return i;
		}
	}
	return -1;
}

}
