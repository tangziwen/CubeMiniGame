#include "RLDirector.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLAIController.h"
#include "RLHeroCollection.h"

namespace tzw
{
RLSubWave::RLSubWave(int wave, int subwaveID)
{
	int wizardCount = (wave) * 1 + subwaveID * 1 + 1;
	int zombieCount = (wave) * 2 + subwaveID * 1 + 2;
	int pigZombieCount = (wave) * 1 + subwaveID * 1 + 1;
	int batCount = subwaveID + 1;
	m_generateMonster[RLHeroCollection::shared()->getHeroIDByName("Wizard")] = wizardCount;
	m_generateMonster[RLHeroCollection::shared()->getHeroIDByName("Zombie")] = zombieCount;
	m_generateMonster[RLHeroCollection::shared()->getHeroIDByName("PigZombie")] = pigZombieCount;
	m_generateMonster[RLHeroCollection::shared()->getHeroIDByName("Bat")] = batCount;
	m_totalCount = wizardCount + zombieCount + pigZombieCount;
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
				hero->equipWeapon(new RLWeapon("MagicBallLauncher"));

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
			if(m_SubWaveIndex > 3)
			{
				m_time = -999999;
			}
		}
		m_time += dt;
	}
	void RLWave::generateSubWaves()
	{
		for(int i = 0; i < 5; i++)
		{
			RLSubWave * subWave = new RLSubWave(m_waveId, i);
			if(i == 0)
			{
				subWave->setWaitingTime(5.f);
			}
			else
			{
				subWave->setWaitingTime(20.f);
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
		for(int i = 0; i < 3; i++)
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
