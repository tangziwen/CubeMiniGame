#include "RLController.h"
#include "RLWorld.h"
#include "RLHero.h"
namespace tzw
{
	RLController::RLController()
	{
	}

	void RLController::possess(RLHero* hero)
	{
		m_currPossessHero = hero;
		hero->setController(this);
	}

	void RLController::tick(float dt)
	{

	}
	RLHero* RLController::getPossessHero()
	{
		return m_currPossessHero;
	}
}