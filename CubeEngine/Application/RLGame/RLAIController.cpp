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

			case RLAIState::WobblyChasing:
			{
				vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
				diff = diff.normalized();
				m_currPossessHero->setPosition(currPos + diff * dt);
			}
			break;
			
			case RLAIState::Chasing:
			{
				vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
				diff = diff.normalized();
				m_currPossessHero->setPosition(currPos + diff * dt * 30);
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

			checkCondition(dt);
		}
	}
	void RLAIController::checkCondition(float dt)
	{
		auto controller = RLWorld::shared()->getPlayerController();
		if(!m_currPossessHero)return;
		if(!controller) return;
		RLHero * self = m_currPossessHero;
		RLHero * player = controller->getPossessHero();
		RLAIState targetState;
		bool isNeedJmp = false;

		//setup context
		RLAICondContext context;
		context.self = self;
		context.target = player;
		vec2 diff = player->getPosition() - self->getPosition();
		context.playerDistance = diff.length();

		for(auto condition : m_statesCondList[int(m_currState)].m_condList)
		{
			condition->tick(dt);
			if(condition->isSatisfied(context))
			{
				isNeedJmp = true;
				targetState = condition->m_targetState;
			}
		}
		if(isNeedJmp)
		{
			//state jmp
			m_currState = targetState;
			//clean up
			for(auto condition : m_statesCondList[int(m_currState)].m_condList)
			{
				condition->resetInternal();
			}
		}
	}
	void RLAIController::addCondition(RLAIState inState, RLAIJmpCond* cond)
	{
		m_statesCondList[int(inState)].m_condList.push_back(cond);
	}



	//RLAIControllerChaser
	RLAIControllerChaser::RLAIControllerChaser()
	{
		float shootRange = 320;
		float shootRangeOffset = 64.f;
		//chasing
		//addCondition(RLAIState::Chasing, new RLAIJmpCondDuration(2.5, RLAIState::Idle, vec2(0.8, 1.6)));

		//idle
		//addCondition(RLAIState::Chasing, new RLAIJmpCondDuration(0.3, RLAIState::Chasing, vec2(0.1, 0.5)));
	}


	//RLAIControllerShooter
	RLAIControllerShooter::RLAIControllerShooter()
	{
		float shootRange = 320;
		float shootRangeOffset = 64.f;
		//chasing
		addCondition(RLAIState::Chasing, new RLAIJmpCondPlayerRange(RLAIJmpCondPlayerRange::Policy::CloseEnough, shootRange - shootRangeOffset, RLAIState::StationaryShooting));
		//shooting
		addCondition(RLAIState::StationaryShooting, new RLAIJmpCondPlayerRange(RLAIJmpCondPlayerRange::Policy::OutOfRange, shootRange + shootRangeOffset, RLAIState::Chasing));
	}
}