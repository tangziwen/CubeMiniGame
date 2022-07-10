#pragma once
#include "2D/Sprite.h"
#include "Engine/EngineDef.h"
#include "RLWeapon.h"
namespace tzw
{
	class RLHero
	{
	public:
		RLHero(int idType);
		void setPosition(vec2 pos);
		const vec2 & getPosition();

		//graphics related
		void updateGraphics();
		void initGraphics();

		void onTick(float dt);

		void equipWeapon(RLWeapon * weapon);
	private:
		vec2 m_pos;
		int m_id = 0;
		Sprite * m_sprite = nullptr;
		bool m_isInitedGraphics = false;
		RLWeapon * m_weapon = nullptr;
	};
}

