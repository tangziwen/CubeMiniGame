#pragma once
#include "2D/Sprite.h"
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
		bool onMouseMove(vec2 pos) override;
	private:
		RLHero * m_currPossessHero = nullptr;
		int m_yAxis = 0, m_xAxis = 0;
		vec2 m_mousePos;
		Sprite * m_crossHairsprite;
		
	};
}
