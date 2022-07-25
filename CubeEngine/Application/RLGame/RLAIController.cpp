#include "RLAIController.h"
#include "RLWorld.h"
#include "RLHero.h"
namespace tzw
{
#define EVERY(duration) ;
	RLAIController::RLAIController()
	{
	}

	void RLAIController::tick(float dt)
	{
		auto controller = RLWorld::shared()->getPlayerController();
		if(m_currPossessHero)
		{
			vec2 currPos = m_currPossessHero->getPosition();

			switch(m_currState)
			{
			case RLAIState::Idle:
			{
				//do nothing
			}
			break;
			case RLAIState::Chasing:
			{
				vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
				diff = diff.normalized();
				m_currPossessHero->setPosition(currPos + diff * dt * 120);
			}
			break;
			case RLAIState::StationaryShooting:
			{
				if(m_currPossessHero->getWeapon())
				{
					vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
					diff = diff.normalized();
					m_currPossessHero->getWeapon()->setShootDir(diff);
					EVERY(5.0)
					{
						m_currPossessHero->getWeapon()->fire();
					}
				}
			}
			break;
			case RLAIState::Charging:
			{
				if(m_isFirstTimeInstate)
				{
					//store the direction in black board

				}
				vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
				diff = diff.normalized();
				m_currPossessHero->setPosition(currPos + diff * dt * 30);
			}
			break;
			case RLAIState::Repositioning:
			{
				if(m_isFirstTimeInstate)
				{
					//store the direction in black board

				}
				vec2 backWardDiff = m_currPossessHero->getPosition() - controller->getPos();
				backWardDiff = backWardDiff.normalized();
				m_currPossessHero->setPosition(currPos + backWardDiff * dt * 90);
			}
			break;
			}
		}
	}



	//RLAIControllerShooter

	void RLAIControllerShooter::tick(float dt)
	{
		auto controller = RLWorld::shared()->getPlayerController();
		if(m_currPossessHero)
		{
			vec2 currPos = m_currPossessHero->getPosition();
			vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
			diff = diff.normalized();
			m_currPossessHero->setPosition(currPos + diff * dt * 30);
			if(m_currPossessHero->getWeapon())
			{
				m_currPossessHero->getWeapon()->setShootDir(diff);
			}
		}
	}
}