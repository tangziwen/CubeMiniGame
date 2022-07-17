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
		if(m_time > 12.0f)
		{
			//genearate a wave
			std::vector<vec2> spawnPos;
			RLWorld::shared()->getRandomBoundaryPos(10, spawnPos);

			for(int i = 0; i < 10; i++)
			{
				RLHero * hero = RLWorld::shared()->spawnHero(1);
				RLAIController * ai = new RLAIController();
				ai->possess(hero);
				hero->setPosition(spawnPos[i]);
			}
			m_time = 0.f;
		}
	}
}