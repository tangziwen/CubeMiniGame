#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	class PTMMapCamera :public EventListener
	{
	public:
		PTMMapCamera(Node * mapRootNode);
		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;
		void onFrameUpdate(float dt) override;
		bool onMouseRelease(int button,vec2 pos) override;
		bool onMousePress(int button,vec2 pos) override;
		bool onMouseMove(vec2 pos) override;
		void init();
	private:
		Node * m_mapRootNode;
		int m_forward;
		int m_slide;
	};

}