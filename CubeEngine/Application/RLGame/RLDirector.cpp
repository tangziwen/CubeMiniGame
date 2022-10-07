#include "RLDirector.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLAIController.h"
#include "RLHeroCollection.h"
#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include "RLShopMgr.h"
namespace tzw
{
void RLSubWaveGenerator::generate(int targetDifficulty, int targetMaxCount, std::unordered_map<int, int>& generateMonster, int & outTotalCount)
{
    auto& re = TbaseMath::getRandomEngine();

	std::vector<int>weightList;
	for(auto & iter : m_info)
	{
		weightList.push_back(iter.weight);
	}
	std::discrete_distribution<> d(weightList.begin(), weightList.end());
	int currentDifficulty = 0;
	for(int i = 0; i < targetMaxCount; i++)
	{
		
		for(int j = 0; j < 50; j++)//max try
		{
			int idx = d(re);
			RLHeroData* hdata =  RLHeroCollection::shared()->getHeroData(m_info[idx].m_monsterName);
			int heroID = RLHeroCollection::shared()->getHeroIDByName(m_info[idx].m_monsterName);
			bool isDifficultyCompatible = (currentDifficulty + hdata->m_difficulty) <= targetDifficulty;
			bool isUnderCountLimit = (generateMonster[heroID] + 1) <= m_info[idx].m_maxCount;
			if(isDifficultyCompatible && isUnderCountLimit)
			{
				//add
				generateMonster[heroID] += 1;
				currentDifficulty += hdata->m_difficulty;
				outTotalCount += 1;
				break;
			}
		}
		if(currentDifficulty > targetDifficulty)//already meet the difficulty
		{
			break;
		}
	}
}
#pragma optimize("", off)
void RLSubWaveGenerator::addMonstersByTiers(int count, int tier, bool ignoreMob)
{
	for(int i = 0; i < count; i ++)
	{
		std::vector<RLHeroData *> heroList;
		RLHeroCollection::shared()->getHeroRangeFromTier("Any", tier,heroList, ignoreMob);
		//erase already added

		for(auto iter = heroList.begin(); iter != heroList.end(); )
		{
			bool isErased = false;
			for(auto info : m_info)
			{
				if(info.m_monsterName == (*iter)->m_name)
				{
					iter = heroList.erase(iter);
					isErased = true;
				}
			}
			if(!isErased)
			{
				iter++;
			}
		}
		/*
		auto isDuplicate = [this](RLHeroData *hero)
		{
			for(auto info : m_info)
			{
				if(info.m_monsterName == hero->m_name)
				{
					return true;
				}
			}
			return false;
		};
		heroList.erase(std::remove_if(heroList.begin(), heroList.end(), isDuplicate));
		*/
		auto & re = TbaseMath::getRandomEngine();
		RLHeroData * addHero =  heroList[ re() % heroList.size()];
		m_info.push_back({addHero->m_name, 6, 25});
	}
}

void RLSubWaveGenerator::removeMonstersByTiers(int tier)
{
	for(auto iter = m_info.begin(); iter != m_info.end() ; )
	{
		RLHeroData * heroDat = RLHeroCollection::shared()->getHeroData(iter->m_monsterName);
		if(heroDat->m_tier == tier)
		{
			iter = m_info.erase(iter);
			return;
		}
		iter++;
	}
}

void RLSubWaveGenerator::removeAllMonsters()
{
	m_info.clear();
}

RLSubWave::RLSubWave(int wave, int subwaveID)
{

}

void RLSubWave::startWave()
	{
		setIsLaunch(true);
		//genearate a wave
		std::vector<vec2> spawnPos;
		RLWorld::shared()->getRandomBoundaryPos(m_totalCount, spawnPos);

		int idx = 0;
		for(auto & iter : m_generateMonster)
		{
			for(int i = 0; i < iter.second; i++)
			{
				RLHero * hero = nullptr;
				hero = RLWorld::shared()->spawnEnemy(iter.first);
				hero->setPosition(spawnPos[idx]);
				hero->setCombatStrengh(RLDirector::shared()->getCombatStrength());
				idx++;
			}

		}

	}
void RLWave::loadStage(std::string filePath)
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
	auto& WavesNode = doc["Waves"];
	for(int i = 0; i < WavesNode.Size(); i++)
	{
		auto& node = WavesNode[i];

		RLWaveGroupInfo group;
		auto& groupNode = node["Group"];
		for(int j = 0; j < groupNode.Size(); j++)
		{
			auto & monsterInfo = groupNode[j];
			std::string typeName = monsterInfo[0].GetString();
			int tier = monsterInfo[1].GetInt();
			int num = monsterInfo[2].GetInt();
			RLMonsterGroupInfo info;
			info.typeName = typeName;
			info.tier = tier;
			info.num = num;
			group.m_groupInfo.push_back(info);
			
		
		}
		group.m_delayTime = node["Delay"].GetFloat();
		m_importantWaveGroup.push_back(group);
	}

	auto& SpecialRandomWavesNode = doc["SpecialRandomWaves"];
	for(int i = 0; i < SpecialRandomWavesNode.Size(); i++)
	{
		auto& node = SpecialRandomWavesNode[i];

		RLWaveGroupInfo group;
		auto& groupNode = node["Group"];
		for(int j = 0; j < groupNode.Size(); j++)
		{
			auto & monsterInfo = groupNode[j];
			std::string typeName = monsterInfo[0].GetString();
			int tier = -1;//monsterInfo[1].GetInt();
			int num = monsterInfo[1].GetInt();
			RLMonsterGroupInfo info;
			info.typeName = typeName;
			info.tier = tier;
			info.num = num;
			group.m_groupInfo.push_back(info);
		
		}
		m_specialRandomWaves.push_back(group);
	}

}
	void RLWave::tick(float dt)
	{

		if(m_SubWaveIndex >= m_SubWaveList.size()) return;//out of sub wave

		if(isFinishedCurrSubWave())
		{
			//go to perk selection
			openPerkSelction();
		}
		else
		{
			if(!m_SubWaveList[m_SubWaveIndex]->getIsLaunch())
			{
				float nextWaveWaitingTime = m_SubWaveList[m_SubWaveIndex]->getWaitingTime();
				if(m_time >= nextWaveWaitingTime)
				{
					launchNextSubWave();
				}
				m_time += dt;
			}
		}
	}

	void RLWave::generateSubWaves()
	{
		auto& re = TbaseMath::getRandomEngine();
		loadStage("RL/Stages.json");
		RLSubWaveGenerator generator;
		


		int wavePerStage = 6;
		int totalWave = 3;
		int BigWaveIdx = 0;
		float prevWaveDelayTime = 0.f;

		for(int i = 0; i < totalWave * wavePerStage; i++)
		{
			RLSubWave * subWave = new RLSubWave(m_waveId, i);

			int unitDifficulty = i * 2 + 1;


			int currStage = i / (wavePerStage);
			int curWaveInStage = i %(wavePerStage);
			//the last mini wave in current wave --> Big Wave
			if(true)
			{
				subWave->m_totalCount = 0;
				RLWaveGroupInfo group =  m_importantWaveGroup[i];
				for(RLMonsterGroupInfo & mInfo : group.m_groupInfo)
				{
					std::vector<RLHeroData * > heroList;
					RLHeroCollection::shared()->getHeroRangeFromTier(mInfo.typeName, mInfo.tier, heroList, false);

					for(int numI=0; numI < mInfo.num; numI ++)
					{
					
						auto hero = heroList[re() %heroList.size()];
						auto iter = subWave->m_generateMonster.find(hero->m_id);
						if(iter == subWave->m_generateMonster.end())
						{
							subWave->m_generateMonster[hero->m_id] = 1;
						}
						else
						{
							subWave->m_generateMonster[hero->m_id] += 1;
						}
						subWave->m_totalCount += 1;
					}
				}
				prevWaveDelayTime = group.m_delayTime;
				BigWaveIdx++;
				if(i == 0)
				{
					subWave->setWaitingTime(5.f);
				}
				else
				{
				
					subWave->setWaitingTime(5.f);
					prevWaveDelayTime = 0.f;
				}
			}

			//test
			subWave->m_generateMonster.clear();
			subWave->m_generateMonster[RLHeroCollection::shared()->getHeroIDByName("Wizard")] = 2;
			subWave->m_totalCount = 2;
			//
			m_SubWaveList.push_back(subWave);
		}
	}

	bool RLWave::isFinished()
	{
		return (m_SubWaveIndex >= m_SubWaveList.size()) && (RLWorld::shared()->getHeroesCount()<= 1);
	}

	void RLWave::openPerkSelction()
	{
		RLShopMgr::shared()->open(m_SubWaveIndex);

	}

	void RLWave::launchNextSubWave()
	{
		m_SubWaveList[m_SubWaveIndex]->startWave();
		m_time = 0;
		m_killRegs.clear();
		RLDirector::shared()->addCombatStrength(0.25f);
	}

	void RLWave::regKill(int heroID)
	{
		auto iter = m_killRegs.find(heroID);
		if(iter == m_killRegs.end())
		{
			m_killRegs[heroID] = 1;
		}
		else
		{
			iter->second += 1;
		}
	}

	bool RLWave::isFinishedCurrSubWave()
	{
		auto & generateMap = m_SubWaveList[m_SubWaveIndex]->getGeneratedMonsterList();

		for(auto iter : generateMap)
		{
			auto killIter = m_killRegs.find(iter.first);
			if(killIter == m_killRegs.end())
			{
				return false;
			}
			if(killIter->second != iter.second)
			{
				return false;
			}
		}
		return true;
	}

	void RLWave::startNextSubWave()
	{
		m_SubWaveIndex += 1;
		m_killRegs.clear();
	}


	RLDirector::RLDirector()
	{

	}

	void RLDirector::tick(float dt)
	{

		if(isFinished() && RLWorld::shared()->getHeroesCount()<= 1)
		{
			RLWorld::shared()->goToWin();
			return;
		}
		if(m_waveIndex >= m_waveList.size()) return;
		m_waveList[m_waveIndex]->tick(dt);
		if(m_waveList[m_waveIndex]->isFinished())
		{
			m_waveIndex ++;
		}

	}
	void RLDirector::startWave()
	{

	}

	void RLDirector::startNextSubWave()
	{
		RLWorld::shared()->resumeGame();
		m_waveList[m_waveIndex]->startNextSubWave();
	}

	void RLDirector::generateWave()
	{
		for(int i = 0; i < 1; i++)
		{
			RLWave * wave = new RLWave(i);
			wave->generateSubWaves();
			m_waveList.push_back(wave);
		}
	}

	int RLDirector::getCurrentWave()
	{
		return m_waveIndex;
	}

	int RLDirector::getCurrentSubWave()
	{
		return m_waveList[m_waveIndex]->getSubWaveIndex() - 1;
	}

	bool RLDirector::isFinished()
	{
		return m_waveIndex >= m_waveList.size();
	}





}
