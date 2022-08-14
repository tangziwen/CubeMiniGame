#include "RLAIController.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLUtility.h"
#include "RLSkills.h"
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
				m_currPossessHero->doMove(diff, dt);
			}
			break;
			
			case RLAIState::StationaryShooting:
			{
				if(m_currPossessHero->getWeapon())
				{
					vec2 diff = controller->getPos() - m_currPossessHero->getPosition();
					diff = diff.normalized();
					m_currPossessHero->getWeapon()->setShootDir(diff);
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
					vec2 chargeDiff =  controller->getPos() - m_currPossessHero->getPosition();
					float chargeDistance = m_blackBoard.getData("ChargeDistance", 300.f);
					//if(chargeDiff.length() > chargeDistance)
					//no matter how far just charge a fixed distance
					{
						chargeDiff = chargeDiff.normalized();
						chargeDiff *= chargeDistance;

					}
					m_blackBoard.writeData("ChargeTarget", m_currPossessHero->getPosition() + chargeDiff);
				}
				vec2 target = m_blackBoard.getData("ChargeTarget", vec2());
				vec2 diff =  target - m_currPossessHero->getPosition();


				if(diff.length() < 16.f)//close enough
				{
					setCurrentTranscationFinish();//mark current transcation finish
				}
				else
				{
				
					diff = diff.normalized();
					m_currPossessHero->setPosition(currPos + diff * dt * 300);
				}
			}
			break;
			case RLAIState::Skilling:
			{
				if(m_isFirstTimeInstate)
				{

					RLSkillBase * skill =  m_currPossessHero->playSkillToTarget(controller->getPossessHero());
					skill->addCallBack([this](RLSkillBase * skill)
						{
							setCurrentTranscationFinish();//mark current transcation finish
						});
				}
			}
			break;
			case RLAIState::FallBack:
			{
				if(m_isFirstTimeInstate|| m_blackBoard.getData("recalFallBackTarget", true))
				{
					//store the direction in black board
					vec2 diff = m_currPossessHero->getPosition() - controller->getPos();
					float length = diff.length();
					diff = diff.normalized();
					Matrix44 mat;
					Quaternion quat;
					if (TbaseMath::randRange(0.0f, 1.0f) > 0.5f)
					{
						quat.fromEulerAngle(vec3(0, 0, TbaseMath::randRange(5, 30)));
					}
					else
					{

						quat.fromEulerAngle(vec3(0, 0, TbaseMath::randRange(-5, -30)));
					}
					
					mat.setRotation(quat); 
					vec4 newDir = mat * vec4(diff.x, diff.y, 0.0, 0.0);
					vec2 repositionTarget = m_currPossessHero->getPosition() + vec2(newDir.x, newDir.y) * m_blackBoard.getData("fallBackDistance", 100.f);
					RLUtility::shared()->clampToBorder(repositionTarget);
					m_blackBoard.writeData("fallBackTarget", repositionTarget);
					//m_recalRepositionTarget = false;
					m_blackBoard.writeData("recalFallBackTarget", false);

				}
				vec2 diff = m_blackBoard.getData("fallBackTarget", vec2()) - m_currPossessHero->getPosition();
				if(diff.length() < 16.f)//close enough
				{
					setCurrentTranscationFinish();//mark current transcation finish
					//recalculate
					m_blackBoard.writeData("recalFallBackTarget", true);
				}
				else
				{
				
					diff = diff.normalized();
					m_currPossessHero->setPosition(currPos + diff * dt * 30);
				}
			}
			break;
			case RLAIState::Repositioning:
			{
				if(m_isFirstTimeInstate || m_blackBoard.getData("recalRepositionTarget", true))
				{
					//store the direction in black board
					vec2 diff = m_currPossessHero->getPosition() - controller->getPos();
					float length = diff.length();
					diff = diff.normalized();
					Matrix44 mat;
					Quaternion quat;
					if (TbaseMath::randRange(0.0f, 1.0f) > 0.5f)
					{
						quat.fromEulerAngle(vec3(0, 0, TbaseMath::randRange(15, 75)));
					}
					else
					{

						quat.fromEulerAngle(vec3(0, 0, TbaseMath::randRange(-15, -75)));
					}
					
					mat.setRotation(quat);
					vec4 newDir = mat * vec4(diff.x, diff.y, 0.0, 0.0);
					vec2 repositionTarget = controller->getPos() + vec2(newDir.x, newDir.y) * length;
					RLUtility::shared()->clampToBorder(repositionTarget);
					m_blackBoard.writeData("repositionTarget", repositionTarget);
					//m_recalRepositionTarget = false;
					m_blackBoard.writeData("recalRepositionTarget", false);

				}
				vec2 diff = m_blackBoard.getData("repositionTarget", vec2()) - m_currPossessHero->getPosition();
				if(diff.length() < 0.1f)//close enough
				{
					setCurrentTranscationFinish();//mark current transcation finish
					//recalculate
					m_blackBoard.writeData("recalRepositionTarget", true);
				}
				else
				{
				
					diff = diff.normalized();
					m_currPossessHero->setPosition(currPos + diff * dt * 30);
				}

			}
			break;
			}

			checkCondition(dt);
		}
	}
	void RLAIController::checkCondition(float dt)
	{
		m_isFirstTimeInstate = false;
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
		context.controller = this;
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
			m_isFirstTimeInstate = true;
		}
		resetAllTranscationFinish();
	}
	void RLAIController::addCondition(RLAIState inState, RLAIJmpCond* cond)
	{
		m_statesCondList[int(inState)].m_condList.push_back(cond);
	}

	void RLAIController::setCurrentTranscationFinish()
	{
		m_isTranscationFinished[int(m_currState)] = true;
	}

	bool RLAIController::isCurrStateTranscationFinish()
	{
		return m_isTranscationFinished[int(m_currState)];
	}

	void RLAIController::resetAllTranscationFinish()
	{
		for(size_t i = 0; i < m_isTranscationFinished.size(); i++)
		{
			m_isTranscationFinished[i] = false;
		}
	}



	//RLAIControllerChaser
	RLAIControllerChaser::RLAIControllerChaser()
	{
		float shootRange = 320;
		float shootRangeOffset = 64.f;
		//chasing
		addCondition(RLAIState::Chasing, new RLAIJmpCondDuration(3.0f, RLAIState::Idle, vec2(0.3f, 1.0f)));

		//idle
		addCondition(RLAIState::Idle, new RLAIJmpCondDuration(0.2f, RLAIState::Chasing, vec2(0.2f, 0.4f)));
	}


	//RLAIControllerShooter
	RLAIControllerShooter::RLAIControllerShooter()
	{
		float shootRange = 320;
		float shootRangeOffset = 64.f;
		//chasing
		addCondition(RLAIState::Chasing, new RLAIJmpCondPlayerRange(RLAIJmpCondPlayerRange::Policy::CloseEnough, shootRange - shootRangeOffset, RLAIState::StationaryShooting));


		//shooting too far
		addCondition(RLAIState::StationaryShooting, new RLAIJmpCondPlayerRange(RLAIJmpCondPlayerRange::Policy::OutOfRange, shootRange + shootRangeOffset, RLAIState::Chasing));
		//shooting too close
		addCondition(RLAIState::StationaryShooting, new RLAIJmpCondPlayerRange(RLAIJmpCondPlayerRange::Policy::CloseEnough, 120, RLAIState::FallBack));
		addCondition(RLAIState::StationaryShooting, new RLAIJmpCondDuration(0.9, RLAIState::Repositioning, vec2(0.5, 1.0)));

		//repositioning
		addCondition(RLAIState::Repositioning, new RLAIJmpCondDuration(1.5, RLAIState::Chasing, vec2(0.8, 1.4)));


		//Fall Back
		addCondition(RLAIState::FallBack, new RLAIJmpCondPlayerRange(RLAIJmpCondPlayerRange::Policy::OutOfRange, shootRange, RLAIState::StationaryShooting));
	}


	RLAIControllerCharger::RLAIControllerCharger()
	{
		m_currState = RLAIState::Skilling;
		//Charging
		addCondition(RLAIState::Skilling, new RLAITranscationCond(RLAIState::Idle));

		//idle
		addCondition(RLAIState::Idle, new RLAIJmpCondDuration(0.8, RLAIState::Skilling, vec2(0.4, 0.6)));
	}

	RLAIController* CreateAIController(std::string AIType)
	{
		
		if(AIType == "None")
		{
			return nullptr;
		}
		else if(AIType == "Chaser")
		{
			return new RLAIControllerChaser();
		}
		else if(AIType == "Shooter")
		{
			return new RLAIControllerShooter();
		}
		else if(AIType == "Charger")
		{
			return new RLAIControllerCharger();
		}
	}
}