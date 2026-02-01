#pragma once
#include "Math/vec2.h"
#include "2D/SpriteInstanceMgr.h"
namespace tzw
{
	class RLInteraction
	{
	public:
		RLInteraction(vec2 pos);
		virtual ~RLInteraction();
		virtual void use();
		virtual void tick(float dt);
		virtual void initGraphics();
		bool isInteractiveable();
	protected:
		SpriteInstanceInfo * m_sprite = nullptr;
		vec2 m_pos;
		bool m_isInteractiveable = false;
	};

	class RLStair : public RLInteraction
	{
	public:
		RLStair(vec2 pos):RLInteraction(pos){};
		virtual void use() override;
	};

}