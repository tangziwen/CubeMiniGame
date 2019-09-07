#include "ItemMgr.h"
#include "3D/Primitive/CubePrimitive.h"
#include <algorithm>
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "CylinderPart.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include <utility>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Utility/log/Log.h"
#include "Utility/file/Tfile.h"

namespace tzw
{
const float bearingGap = 0.00;
TZW_SINGLETON_IMPL(ItemMgr);
ItemMgr::ItemMgr()
{
	//load from Json
	loadFromFile("./Res/Items.json");
}


void ItemMgr::loadFromFile(std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d\n", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
		exit(1);
	}
	if (doc.HasMember("Items"))
	{
		auto& items = doc["Items"];
		for (unsigned int i = 0; i < items.Size(); i++)
		{
			auto& item = items[i];
			auto name = item["name"].GetString();
			auto type = item["type"].GetInt();
			auto desc = item["desc"].GetString();
			auto class_s = item["class"].GetString();
			auto gameItem = new GameItem();
			gameItem->m_name = name;
			gameItem->m_type = type;
			gameItem->m_desc = desc;
			gameItem->m_class = class_s;
			m_itemMap[name] = gameItem;
		}
	}
}

GameItem * ItemMgr::getItem(std::string name)
{
	return m_itemMap[name];
}

}
