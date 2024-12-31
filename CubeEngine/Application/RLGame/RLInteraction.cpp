#include "RLInteraction.h"
#include "RLSpritePool.h"
#include "RLWorld.h"
#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include <algorithm>
#include "Utility/math/TbaseMath.h"
#include "RLPlayerState.h"
namespace tzw
{



RLInteraction::RLInteraction(vec2 pos):m_pos(pos)
{
}

RLInteraction::~RLInteraction()
{
	RLSpritePool::shared()->get()->removeSprite(m_sprite);
}

void RLInteraction::use()
{
}

void RLInteraction::tick(float dt)
{
	m_isInteractiveable = false;
	vec2 pos = RLWorld::shared()->getPlayerController()->getPos();
	if((m_pos- pos).length() < 64.f)
	{
		m_isInteractiveable = true;
	}
}

void RLInteraction::initGraphics()
{
	int spriteType = RLSpritePool::shared()->get()->getOrAddType("RL/Sprites/Hatch.png");
	SpriteInstanceInfo * info = new SpriteInstanceInfo();
	info->type = spriteType;
	m_sprite = info;
	m_sprite->layer = 0;
	m_sprite->pos = m_pos;
	RLSpritePool::shared()->get()->addTile(m_sprite);
}

bool RLInteraction::isInteractiveable()
{
	return m_isInteractiveable;
}

void RLStair::use()
{
	RLWorld::shared()->startNextSubWave();
}

void RLReward::initGraphics()
{
	int spriteType = RLSpritePool::shared()->get()->getOrAddType(m_data->m_res);
	SpriteInstanceInfo * info = new SpriteInstanceInfo();
	info->type = spriteType;
	m_sprite = info;
	m_sprite->layer = 0;
	m_sprite->pos = m_pos;
	RLSpritePool::shared()->get()->addTile(m_sprite);
}

void RLReward::use()
{
	m_isAlive = false;
	auto ps = RLPlayerState::shared();
	if(m_data->m_type == RLRewardItemData::ItemType::StatsMod)
	{
		for(int i = 0; i < 3; i++)
		{
			ps->addStat(i, m_data->m_stats[i]);
		}
	}
	//RLWorld::shared()->startNextSubWave();
}



void RLRewardItemMgr::loadConfig()
{
	std::string filePath = "RL/Items.json";
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
		RLRewardItemData data;
		data.m_name = node["Name"].GetString();
		std::string typeStr = node["type"].GetString();

		if(typeStr == "stat_mod")
		{
			auto& statArray = node["stat"];
			data.m_stats[0] = statArray[0].GetInt();
			data.m_stats[1] = statArray[1].GetInt();
			data.m_stats[2] = statArray[2].GetInt();
			data.m_type = RLRewardItemData::ItemType::StatsMod;
		}
		data.m_res = node["texture"].GetString();
		m_data.emplace_back(data);
	}
}

RLRewardItemData * RLRewardItemMgr::getNextItem()
{
	auto& re = TbaseMath::getRandomEngine();
	return &m_data[re()%m_data.size()];
}
}