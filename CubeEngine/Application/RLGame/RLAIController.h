#pragma once
#include <array>
#include <vector>
#include "RLController.h"
#include "Math/vec2.h"
#include "Utility/math/TbaseMath.h"
#include "Base/BlackBoard.h"
namespace tzw
{
	class RLHero;
	class RLAIController;
	enum class RLAIState
	{
		Idle =0,
		Chasing,
		StationaryShooting,
		Charging,
		Repositioning,
		WobblyChasing,
		FallBack,
		TotalEnum,
	
	};
	struct RLAICondContext
	{
		RLHero * self = nullptr;
		RLHero * target = nullptr;
		RLAIController * controller = nullptr;
		float playerDistance = 0.f;
	};
	struct RLAIJmpCond
	{
		RLAIJmpCond(RLAIState targetState):m_targetState(targetState){};
		RLAIState m_targetState;
		virtual bool isSatisfied(const RLAICondContext & context) = 0;
		virtual void tick(float dt)  = 0;
		virtual void resetInternal() = 0;
	};

	struct RLAIJmpCondList
	{
		std::vector<RLAIJmpCond *> m_condList;
	};


	

	struct RLAIJmpCondDuration : public RLAIJmpCond
	{
		RLAIJmpCondDuration(float duration, RLAIState targetState, vec2 randomRange = vec2(0.1, 0.5))
			:RLAIJmpCond(targetState), m_duration(duration)
		{
			caculateRandomOffset();
		};
		virtual bool isSatisfied(const RLAICondContext & context) override 
		{
			return m_currTime >= m_duration + m_randomOffset;
		};
		virtual void tick(float dt) override 
		{
			m_currTime += dt;
		};
		virtual void resetInternal() override
		{
			m_currTime = 0.f;
			caculateRandomOffset();
		}
		void caculateRandomOffset()
		{
			m_randomOffset = TbaseMath::randRange(m_randomRange.x, m_randomRange.y);
		}
	private:
		float m_currTime = 0.f;
		float m_duration = 1.0f;
		float m_randomOffset = 0.0f;
		vec2 m_randomRange = vec2(0.1, 0.5);
	};


	struct RLAIJmpCondPlayerRange : public RLAIJmpCond
	{
		enum class Policy
		{
			OutOfRange,
			CloseEnough,
		};
		RLAIJmpCondPlayerRange(Policy policy, float distance, RLAIState targetState)
			:RLAIJmpCond(targetState), m_distance(distance), m_policy(policy)
		{};
		virtual bool isSatisfied(const RLAICondContext & context) override 
		{
			switch (m_policy)
			{
			case Policy::CloseEnough:
				return context.playerDistance < m_distance;
				break;
			case Policy::OutOfRange:
				return context.playerDistance >= m_distance;
				break;
			}
			return false;
		};
		virtual void tick(float dt) override 
		{
			
		};
		virtual void resetInternal() override
		{

		}
	private:

		float m_distance = 1.0f;
		Policy m_policy = Policy::CloseEnough;
	};

	class RLAIController : public RLController
	{
	public:
		RLAIController();
		virtual void tick(float dt) override;
		void addCondition(RLAIState inState, RLAIJmpCond* cond);
		void checkCondition(float dt);
		bool isCurrStateTranscationFinish();
	protected:
		void setCurrentTranscationFinish();
		void resetAllTranscationFinish();
		RLAIState m_currState = RLAIState::Chasing;
		bool m_isFirstTimeInstate = true;
		std::array<RLAIJmpCondList, size_t(RLAIState::TotalEnum)> m_statesCondList;
		std::array<bool, size_t(RLAIState::TotalEnum)> m_isTranscationFinished;
		BlackBoard m_blackBoard;
	};


	class RLAIControllerChaser : public RLAIController
	{
	public:
		RLAIControllerChaser();

	};

	class RLAIControllerShooter : public RLAIController
	{
	public:
		RLAIControllerShooter();

	};


	struct RLAITranscationCond : public RLAIJmpCond
	{
		RLAITranscationCond(RLAIState targetState)
			:RLAIJmpCond(targetState)
		{
		
		}
		virtual bool isSatisfied(const RLAICondContext & context) override 
		{
			return context.controller->isCurrStateTranscationFinish();
		};
	};
}