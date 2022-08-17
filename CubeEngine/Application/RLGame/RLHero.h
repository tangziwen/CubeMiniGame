#pragma once
#include "2D/Sprite.h"
#include "Engine/EngineDef.h"
#include "RLWeapon.h"
#include "2D/Collider2D.h"
#include "2D/SpriteInstanceMgr.h"
#include "RLEffectMgr.h"
namespace tzw
{
	class RLController;
	class RLHeroData;
	class RLSkillBase;
	class RLHero : public TObjectReflect
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
		void onPossessed();
		float m_speed = 0.f;
		void doDash();
		void doMove(vec2 dir, float delta);
		void applyEffect(std::string name);
		void applyEffect(RLEffect * effect);
		RLHeroData * getHeroData();
		RLSkillBase* playSkill();
		RLSkillBase* playSkillToTarget(RLHero * hero);
		TZW_PROPERTY(float, HP, 100.f);
		TZW_PROPERTY(float, MAXHP, 100.f);
		TZW_PROPERTY(float, Mana, 100.f);
		TZW_PROPERTY(float, MaxMana, 100.f);
		TZW_PROPERTY(float, BaseDamage, 0.f);

	T_PROP_REFLECT_REG_DECLEAR()
		T_PROP_REFLECT_REG(HP)
		T_PROP_REFLECT_REG(MAXHP)
		T_PROP_REFLECT_REG(Mana)
		T_PROP_REFLECT_REG(MaxMana)
	T_PROP_REFLECT_REG_END
	private:
		RLSkillBase * m_currSkill = nullptr;
		RLEffectContainer m_container;
		//vec2 m_pos;
		int m_id = 0;
		SpriteInstanceInfo * m_sprite = nullptr;
		bool m_isInitedGraphics = false;
		RLWeapon * m_weapon = nullptr;
		Collider2D * m_collider = nullptr;
		bool m_isPlayerControll = false;
		float m_hitImmuneTimer = 0.f;
		bool m_isHitImmune = false;
		RLController * m_controller = nullptr;
		RLHeroData * m_heroData;
		bool m_triggerHitEffect = false;
		float m_hitEffectTimer = 0.f;


		vec2 m_moveDir;
		bool m_isMoving = false;

		bool m_isDash = false;
		float m_dashTimer = 0.f;
		vec2 m_dashVelocity;
		vec2 m_dashDir;
		float m_dashSpeed;
	};
}

