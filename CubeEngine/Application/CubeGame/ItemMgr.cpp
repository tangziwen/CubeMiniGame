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
#include "Rendering/Renderer.h"

namespace tzw
{
const float bearingGap = 0.00;
ItemMgr::ItemMgr()
{
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
	if (doc.HasMember("Parts"))
	{
		auto& items = doc["Parts"];
		for (unsigned int i = 0; i < items.Size(); i++)
		{
			auto& item = items[i];
			auto name = item["name"].GetString();
			std::string type = item["Type"].GetString();
			auto desc = item["desc"].GetString();
			auto description = item["description"].GetString();
			auto gameItem = new GameItem();
			gameItem->m_name = name;
			bool isSpecial = false;
			if(type == "GAME_PART_BLOCK")
			{
				gameItem->m_type = GamePartType::GAME_PART_BLOCK;
			}
			if(type == "GAME_PART_LIFT")
			{
				gameItem->m_type = GamePartType::GAME_PART_LIFT;
			}
			if(type == "GAME_PART_CONTROL")
			{
				gameItem->m_type = GamePartType::GAME_PART_CONTROL;
			}
			if(type == "GAME_PART_THRUSTER")
			{
				gameItem->m_type = GamePartType::GAME_PART_THRUSTER;
			}
			if(type == "GAME_PART_CANNON")
			{
				gameItem->m_type = GamePartType::GAME_PART_CANNON;
			}
			if(type == "GAME_PART_BEARING")
			{
				gameItem->m_type = GamePartType::GAME_PART_BEARING;
			}
			if(type == "GAME_PART_SPRING")
			{
				gameItem->m_type = GamePartType::GAME_PART_SPRING;
			}
			if(type == "SPECIAL_PART_PAINTER")
			{
				gameItem->m_type = GamePartType::SPECIAL_PART_PAINTER;
				isSpecial = true;
			}
			if(type == "SPECIAL_PART_DIGGER")
			{
				gameItem->m_type = GamePartType::SPECIAL_PART_DIGGER;
				isSpecial = true;
			}
			if(type == "GAME_PART_BUTTON")
			{
				gameItem->m_type = GamePartType::GAME_PART_BUTTON;
			}
			if(type == "GAME_PART_SWITCH")
			{
				gameItem->m_type = GamePartType::GAME_PART_SWITCH;
			}
			gameItem->m_desc = desc;
			gameItem->m_description = description;
			if(!isSpecial)
			{
				auto& attachs = item["attach"];
				for(int i = 0; i < attachs.Size(); i++)
				{
					auto& attachData = attachs[i];
					vec3 pos = vec3(attachData["pos"][0].GetDouble(), attachData["pos"][1].GetDouble(), attachData["pos"][2].GetDouble());
					vec3 normal = vec3(attachData["normal"][0].GetDouble(), attachData["normal"][1].GetDouble(), attachData["normal"][2].GetDouble());
					vec3 up = vec3(attachData["up"][0].GetDouble(), attachData["up"][1].GetDouble(), attachData["up"][2].GetDouble());
					std::string locale = "down";
					if(attachData.HasMember("locale")) 
					{
						locale = attachData["locale"].GetString();
					}
					float collisionSize = item["CollisionSize"].GetDouble();
					gameItem->m_attachList.push_back(AttachmentInfo(pos, normal, up, locale, collisionSize));
				}

				auto& visualData = item["Visual"];
				{
					std::string visualInfoType = visualData["Type"].GetString();
					vec3 visualSize = vec3(visualData["size"][0].GetDouble(), visualData["size"][1].GetDouble(), visualData["size"][2].GetDouble());
					if(visualInfoType == "CubePrimitive")
					{
						gameItem->m_visualInfo.type = VisualInfo::VisualInfoType::CubePrimitive;
					}
					else if(visualInfoType == "CylinderPrimitive")
					{
						gameItem->m_visualInfo.type = VisualInfo::VisualInfoType::CylinderPrimitive;
					}
					else if(visualInfoType == "RightPrismPrimitive")
					{
						gameItem->m_visualInfo.type = VisualInfo::VisualInfoType::RightPrismPrimitive;
					}
					else if(visualInfoType == "Mesh")
					{
						gameItem->m_visualInfo.type = VisualInfo::VisualInfoType::Mesh;
						gameItem->m_visualInfo.filePath = visualData["FilePath"].GetString();
						gameItem->m_visualInfo.diffusePath = visualData["DiffusePath"].GetString();

						if(visualData.HasMember("RoughnessPath"))
						{
							gameItem->m_visualInfo.roughnessPath = visualData["RoughnessPath"].GetString();
						} else
						{
							gameItem->m_visualInfo.roughnessPath = "Texture/BuiltInTexture/defaultRoughnessMap.png";
						}

						if(visualData.HasMember("MetallicPath"))
						{
							gameItem->m_visualInfo.metallicPath = visualData["MetallicPath"].GetString();
						} else
						{
							gameItem->m_visualInfo.metallicPath = "Texture/BuiltInTexture/defaultMetallic.png";
						}

						if(visualData.HasMember("NormalMapPath")) 
						{
							gameItem->m_visualInfo.normalMapPath = visualData["NormalMapPath"].GetString();
						}
						else
						{
							gameItem->m_visualInfo.normalMapPath = "Texture/BuiltInTexture/defaultNormalMap.png";
						}
						if(visualData.HasMember("ExtraMeshList"))
						{
							for(int k = 0; k < visualData["ExtraMeshList"].Size(); k++)
							{
								gameItem->m_visualInfo.extraFileList.push_back(visualData["ExtraMeshList"][0].GetString());
							}
						}
					}
					gameItem->m_visualInfo.size = visualSize;
				}


				auto& physicsData = item["Physics"];
				{
					std::string physicsInfoType = physicsData["Type"].GetString();
					vec3 physicsSize = vec3(physicsData["size"][0].GetDouble(), physicsData["size"][1].GetDouble(), physicsData["size"][2].GetDouble());
					if(physicsInfoType == "BoxShape")
					{
						gameItem->m_physicsInfo.type = PhysicsInfo::PhysicsInfoType::BoxShape;
					}
					else if(physicsInfoType == "CylinderShape")
					{
						gameItem->m_physicsInfo.type = PhysicsInfo::PhysicsInfoType::CylinderShape;
					}
					gameItem->m_physicsInfo.size = physicsSize;
					if(physicsData.HasMember("Mass"))
					{
						gameItem->m_physicsInfo.mass = physicsData["Mass"].GetDouble();
					}else
					{
						gameItem->m_physicsInfo.mass = 0.5f;
					}
				}
			}

			if(!gameItem->isSpecialFunctionItem()) 
			{
				auto part = new GamePart();
				part->initFromItem(gameItem);

				gameItem->m_thumbNail = new ThumbNail(part->getNode());
				Renderer::shared()->updateThumbNail(gameItem->m_thumbNail);
			} else
			{
				gameItem->m_thumbNail = nullptr;
			}
			if(item.HasMember("Icon"))
			{
				gameItem->m_texture = TextureMgr::shared()->getByPath(item["Icon"].GetString());
			}

			if(item.HasMember("TintColor"))
			{
				gameItem->m_tintColor = vec3(item["TintColor"][0].GetDouble(), item["TintColor"][1].GetDouble(), item["TintColor"][2].GetDouble());
			}

			if(item.HasMember("PartSurface"))
			{
				gameItem->m_surfaceName = item["PartSurface"].GetString();
			}
			m_itemMap[name] = gameItem;
			m_itemList.push_back(gameItem);
		}
	}
}

GameItem * ItemMgr::getItem(std::string name)
{
	return m_itemMap[name];
}

int ItemMgr::getItemAmount()
{
	return m_itemList.size();
}

GameItem* ItemMgr::getItemByIndex(int index)
{
	return m_itemList[index];
}

void ItemMgr::pushItem(GameItem* item)
{
	m_itemMap[item->m_name] = item;
	m_itemList.push_back(item);
}
}
