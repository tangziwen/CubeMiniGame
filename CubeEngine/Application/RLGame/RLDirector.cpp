#include "RLDirector.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLAIController.h"
namespace tzw
{
	RLDirector::RLDirector()
	{

	}

	void RLDirector::tick(float dt)
	{
		m_time += dt;
		if(m_time > 30.f)
		{
			startWave();
			m_time = 0.f;
		}
	}
	void RLDirector::startWave()
	{
		//genearate a wave
		std::vector<vec2> spawnPos;
		RLWorld::shared()->getRandomBoundaryPos(10, spawnPos);

		for(int i = 0; i < 8; i++)
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
		m_time = 0.f;
	}
}