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
		if(m_time > 10.0f)
		{
			//genearate a wave
			RLHero * hero = RLWorld::shared()->spawnHero(0);
			RLAIController * ai = new RLAIController();
			ai->possess(hero);
			hero->setPosition(vec2(200, 50));
			m_time = 0.f;
		}
	}
}