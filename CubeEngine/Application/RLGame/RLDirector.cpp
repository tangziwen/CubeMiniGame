#include "RLDirector.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLAIController.h"
#include "RLHeroCollection.h"

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
		RLHeroCollection::shared()->getHeroRangeFromTier(tier,heroList, ignoreMob);
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
				idx++;
			}

		}

	}
	void RLWave::tick(float dt)
	{
		if(m_SubWaveIndex >= m_SubWaveList.size()) return;//out of sub wave

		if(m_time >= m_SubWaveList[m_SubWaveIndex]->getWaitingTime())
		{
			m_SubWaveList[m_SubWaveIndex]->startWave();
			m_SubWaveIndex ++;
			m_time = 0;
			//if(m_SubWaveIndex > 3)
			{
			}
		}
		m_time += dt;
	}
	void RLWave::generateSubWaves()
	{

		RLSubWaveGenerator generator;
		

		int miniWavePerWave = 15;
		int wavePerTier = 7;
		int totalWave = 3;
		int addMonsterCount = 0;

		for(int i = 0; i < totalWave * wavePerTier * miniWavePerWave; i++)
		{
			RLSubWave * subWave = new RLSubWave(m_waveId, i);
			int unitDifficulty = i * 2 + 1;


			int currTier = i / (wavePerTier * miniWavePerWave);
			int currMiniWaveInTier = i %(miniWavePerWave * wavePerTier);
			int curWaveInTier = currMiniWaveInTier / miniWavePerWave;
			int curMiniWaveInWave = currMiniWaveInTier % miniWavePerWave;

			//the last mini wave in current wave --> Big Wave
			if(curMiniWaveInWave % miniWavePerWave == (miniWavePerWave - 1))
			{
				if(curWaveInTier == 0)
				{
					generator.removeAllMonsters();
					generator.addMonstersByTiers(3, currTier, true);
				}
				int transitionWaves = 2;
				if(curWaveInTier >= (wavePerTier-transitionWaves))//the last two wave in current tier, make transition big wave
				{
					if(currTier + 1 < totalWave)
					{
						generator.addMonstersByTiers(1, currTier + 1, true);
						generator.removeMonstersByTiers(currTier);
					}

				}
				int totalCount = std::min(currTier * 1 + 1 + curWaveInTier, 6);
				int totalDifficulty = 20 + currTier * 100 + curWaveInTier * 15;
				generator.generate(totalDifficulty, totalCount, subWave->m_generateMonster, subWave->m_totalCount);
			}
			else//mini wave
			{
				std::vector<RLHeroData * > mobList;
				RLHeroCollection::shared()->getMobsFromTiers(currTier, mobList);
				int ThirdOfWave = (miniWavePerWave / 3);
				if(curMiniWaveInWave % miniWavePerWave == ThirdOfWave)// 1 / 3 of wave, spawn a tank
				{
						subWave->m_generateMonster[mobList[0]->m_id] = 1;
						subWave->m_totalCount = 1;
				}
				else if(curMiniWaveInWave % miniWavePerWave == ThirdOfWave * 2)// 2 / 3 of wave, spawn a dog wave
				{
						subWave->m_generateMonster[mobList[1]->m_id] = 5;
						subWave->m_totalCount = 5;
				}
				else
				{


					int MobType = (curWaveInTier / 3) + 2;//0号位留给Tank, 1号位留给疯狗
					int MobCount = 2;
					if(curMiniWaveInWave % 3 == 2)
					{
						subWave->m_generateMonster[mobList[MobType]->m_id] = MobCount + 1;
						subWave->m_totalCount = MobCount + 1;
					}else
					{
						subWave->m_generateMonster[mobList[MobType]->m_id] = MobCount;
						subWave->m_totalCount = MobCount;
					}
				}
	

			}
			//

			if(i == 0)
			{
				subWave->setWaitingTime(5.f);
			}
			else
			{
				
				subWave->setWaitingTime(2.f);
			}
			
			m_SubWaveList.push_back(subWave);
		}
	}

	bool RLWave::isFinished()
	{
		return (m_SubWaveIndex >= m_SubWaveList.size()) && (RLWorld::shared()->getHeroesCount()<= 1);
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
