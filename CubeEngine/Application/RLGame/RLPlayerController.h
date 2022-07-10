#pragma once
#include "Event/Event.h"

namespace tzw
{
	class RLHero;
	class RLPlayerController: public EventListener
	{
	public:
		RLPlayerController();
		void possess(RLHero * hero);
		void processInput(float dt);
		void onFrameUpdate(float dt) override;
		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;
	private:
		RLHero * m_currPossessHero = nullptr;
		int m_yAxis, m_xAxis;
		
	};
}
