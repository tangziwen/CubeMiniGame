#include "RLPlayerState.h"
#include "RLCollectible.h"
#include "RLWorld.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include <memory>
namespace tzw
{
const char * g_PlayerPersistentFile = "./ShooterPlayerConfig.json";
RLPlayerState::RLPlayerState()
{
	reset();
	m_heroPurchasedMap = {{"Cop", 1}, {"Soldier",1}, {"TrenchFighter", 1}};//default unlock heroes.
	loadPersistent();
	
}
void RLPlayerState::reset()
{
	m_score = 0;
	m_currLevel = 1;
	m_currExp = 0;
	m_maxExp = calculateNextLevelExp(m_currLevel + 1);
}
unsigned int RLPlayerState::getScore()
{
	return m_score;
}
void RLPlayerState::setScore(unsigned int newScore)
{
	m_score = newScore;
}
void RLPlayerState::addScore(unsigned int addedScore)
{
	m_score += addedScore;
}
unsigned int RLPlayerState::calculateNextLevelExp(unsigned int nextLevel)
{
	return (m_currLevel + 1) * 25 + 500;
}
void RLPlayerState::gainExp(unsigned int exp)
{
	m_currExp += exp;
	if(m_currExp >= m_maxExp)
	{
		m_currLevel++;//level Up
		RLWorld::shared()->generateLevelUpPerk();
		m_currExp = m_currExp - m_maxExp;
		m_maxExp = calculateNextLevelExp(m_currLevel);
	}
}
unsigned int RLPlayerState::getCurrLevel()
{
	return m_currLevel;
}
unsigned int RLPlayerState::getCurrExp()
{
	return m_currExp;
}
unsigned int RLPlayerState::getMaxExp()
{
	return m_maxExp;
}
void RLPlayerState::addGold(unsigned int newGoldValue)
{
	m_gold += newGoldValue;
}
void RLPlayerState::unlockHero(std::string heroName)
{
	m_heroPurchasedMap[heroName] = 1;
	m_gold -= 100;
	writePersistent();
}

bool RLPlayerState::isHeroUnLock(std::string heroName)
{
	return m_heroPurchasedMap.find(heroName) != m_heroPurchasedMap.end();
}

int RLPlayerState::getHeroLevel(std::string heroName)
{
	auto iter = m_heroPurchasedMap.find(heroName);
	if(iter != m_heroPurchasedMap.end())
	{
		return iter->second;
	}
	else
	{
		return -1;
	}
}

void RLPlayerState::heroUpdate(std::string heroName)
{
	auto iter = m_heroPurchasedMap.find(heroName);
	if(iter != m_heroPurchasedMap.end())
	{
		if(iter->second < 5)
		{
			iter->second += 1;
			m_gold -= 100;
		}
	}
	writePersistent();
}

bool RLPlayerState::isCanAffordHeroPurchase(std::string heroName, HeroPurchaseAction action)
{
	switch (action)
	{
	case tzw::HeroPurchaseAction::Upgrade:
		return m_gold >= 100;
		break;
	case tzw::HeroPurchaseAction::Unlock:
		return m_gold >= 500;
		break;
	default:
		break;
	}
	return false;
}

void RLPlayerState::loadPersistent()
{
	rapidjson::Document doc;
	if(Tfile::shared()->isExist(g_PlayerPersistentFile))
	{
		auto data = Tfile::shared()->getData(g_PlayerPersistentFile, true);
		doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				g_PlayerPersistentFile,
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}
		m_gold = doc["Gold"].GetUint();
		auto& unlockHeroListNode = doc["UnlockHeroList"];
		m_heroPurchasedMap.clear();
		for(int i = 0; i < unlockHeroListNode.Size(); i++)
		{
			m_heroPurchasedMap[unlockHeroListNode[i][0].GetString()] = unlockHeroListNode[i][1].GetInt();
		}
	}
	else//make a new file
	{
		writePersistent();
	}

}
void RLPlayerState::writePersistent()
{
		rapidjson::Document doc;
		doc.SetObject();

		doc.AddMember("Gold", m_gold, doc.GetAllocator());

		rapidjson::Value unlockHeroList(rapidjson::kArrayType);
		for(auto heroIter : m_heroPurchasedMap)
		{
			rapidjson::Value heroObj(rapidjson::kArrayType);
			rapidjson::Value heroStr(rapidjson::kStringType);
			heroStr.SetString(heroIter.first, doc.GetAllocator());
			heroObj.PushBack(heroStr, doc.GetAllocator());
			heroObj.PushBack(heroIter.second, doc.GetAllocator());
			unlockHeroList.PushBack(heroObj, doc.GetAllocator());
		
		}
		doc.AddMember("UnlockHeroList", unlockHeroList, doc.GetAllocator());
		rapidjson::StringBuffer buffer;
		auto file = fopen(g_PlayerPersistentFile, "w");
		auto writeBuffer =  std::make_unique<char[]>(65536);//std::unique_ptr<char[]> p(new char[1000000]);

		rapidjson::FileWriteStream stream(file, writeBuffer.get(), sizeof(65536));
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
		writer.SetIndent('\t', 1);
		doc.Accept(writer);
		fclose(file);
}
}
