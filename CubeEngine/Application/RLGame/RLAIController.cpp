#include "RLAIController.h"
#include "RLWorld.h"
#include "RLHero.h"
namespace tzw
{
	RLAIController::RLAIController()
	{
	}

	void RLAIController::tick(float dt)
	{
		auto controller = RLWorld::shared()->getPlayerController();
		controller->getPos();
		
		if(m_currPossessHero)
		{
			vec2 currPos = m_currPossessHero->getPosition();
			vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
			diff = diff.normalized();
			m_currPossessHero->setPosition(currPos + diff * dt * 20);
		}
	}
}