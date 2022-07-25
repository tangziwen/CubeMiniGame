#pragma once
#include "RLController.h"
namespace tzw
{

	enum class RLAIState
	{
		Idle =0,
		Chasing,
		StationaryShooting,
		Charging,
		Repositioning
	
	};
	class RLHero;
	class RLAIController : public RLController
	{
	public:
		RLAIController();
		virtual void tick(float dt) override;
	protected:
		RLAIState m_currState = RLAIState::Chasing;
		bool m_isFirstTimeInstate = true;
	};


	class RLAIControllerShooter : public RLAIController
	{
	public:
		virtual void tick(float dt) override;
	};
}