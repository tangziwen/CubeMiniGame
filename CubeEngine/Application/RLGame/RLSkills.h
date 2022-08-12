#pragma once
#include "Base/BlackBoard.h"
#include <functional>
namespace tzw
{
	class RLHero;
	class RLSkillBase
	{
	public:
		RLSkillBase(RLHero * hero):m_parent(hero){};
		virtual void onEnter() = 0;
		virtual void onTick(float dt) = 0;
		virtual bool isFinished() = 0;
		BlackBoard & getBlackBoard() {return m_blackBoard;};
		void addCallBack(std::function<void(RLSkillBase *)> callBack);
		void raiseFinished();
	protected:
		BlackBoard m_blackBoard;
		RLHero * m_parent;
		std::vector<std::function<void(RLSkillBase *)> > m_callBackList;
	};



	class RLSkillCharge : public RLSkillBase
	{
	public:
		RLSkillCharge(RLHero * hero):RLSkillBase(hero){};
		virtual void onEnter() override;
		virtual void onTick(float dt) override;
		virtual bool isFinished() override;
	protected:
		bool m_isFinished = false;
	};
}