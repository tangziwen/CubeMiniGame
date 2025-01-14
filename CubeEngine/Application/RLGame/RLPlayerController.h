#pragma once
#include "2D/Sprite.h"
#include "Event/Event.h"
#include "Math/Matrix44.h"
#include "RLController.h"
namespace tzw
{
	class RLHero;
	class RLPlayerController: public RLController, public EventListener
	{
	public:
		RLPlayerController();
		void possess(RLHero * hero) override;
		void processInput(float dt);
		void onFrameUpdate(float dt) override;
		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;
		bool onMouseMove(vec2 pos) override;
		bool onMouseRelease(int button,vec2 pos) override;
		bool onMousePress(int button,vec2 pos) override;
		vec2 getPos();
		void useInteraction();
	private:
		void calculateView();
		int m_yAxis = 0, m_xAxis = 0;
		vec2 m_mousePos;
		
		Matrix44 m_viewMat;
		Matrix44 m_invViewMat;
	};
}
