#pragma once
#include "2D/Sprite.h"
#include "Engine/EngineDef.h"
#include "RLWeapon.h"
#include "2D/Collider2D.h"
namespace tzw
{
	class RLController;
	class RLHero
	{
	public:
		RLHero(int idType);
		~RLHero();
		void setPosition(vec2 pos);
		const vec2 & getPosition();

		//graphics related
		void updateGraphics();
		void initGraphics();

		void onTick(float dt);

		void equipWeapon(RLWeapon * weapon);
		RLWeapon * getWeapon();
		Collider2D * getCollider2D();
		void onCollision(Collider2D * self, Collider2D * other);
		void setIsPlayerControll(bool newVal) {m_isPlayerControll = newVal;}
		bool getIsPlayerControll() {return m_isPlayerControll;}
		void receiveDamage(float damage);
		bool isAlive();
		void setController(RLController * controller);
	private:
		vec2 m_pos;
		int m_id = 0;
		Sprite * m_sprite = nullptr;
		bool m_isInitedGraphics = false;
		RLWeapon * m_weapon = nullptr;
		Collider2D * m_collider = nullptr;
		bool m_isPlayerControll = false;
		float m_hp = 100.f;
		float m_hitTimer = 0.f;
		bool m_isHitImmune = false;
		RLController * m_controller = nullptr;
	};
}

