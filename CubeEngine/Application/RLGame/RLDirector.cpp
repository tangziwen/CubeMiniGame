#include "RLDirector.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLAIController.h"
namespace tzw
{
	void RLSubWave::startWave()
	{
		//genearate a wave
		std::vector<vec2> spawnPos;
		RLWorld::shared()->getRandomBoundaryPos(15, spawnPos);

		for(int i = 0; i < 3; i++)
		{
			RLHero * hero = nullptr;
			if(rand() % 10 > 5)
			{
				hero = RLWorld::shared()->spawnHero(2);
				RLAIController * ai = new RLAIControllerShooter();
				ai->possess(hero);
			}
			else
			{
				hero = RLWorld::shared()->spawnHero(1);
				RLAIController * ai = new RLAIControllerChaser();
				ai->possess(hero);
			}
			hero->equipWeapon(new RLWeapon("MagicBallLauncher"));

			hero->setPosition(spawnPos[i]);
		}
	}
	void RLWave::tick(float dt)
	{
		if(m_SubWaveIndex >= m_SubWaveList.size()) return;//out of sub wave

		if(m_time >= m_SubWaveList[m_SubWaveIndex]->getWaitingTime())
		{
			m_SubWaveList[m_SubWaveIndex]->startWave();
			m_SubWaveIndex ++;
			m_time = 0.f;
		}
		m_time += dt;
	}
	void RLWave::generateSubWaves()
	{
		for(int i = 0; i < 5; i++)
		{
			RLSubWave * subWave = new RLSubWave();
			if(i == 0)
			{
				subWave->setWaitingTime(5.f);
			}
			else
			{
				subWave->setWaitingTime(5.f);
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
		for(int i = 0; i < 5; i++)
		{
			RLWave * wave = new RLWave();
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