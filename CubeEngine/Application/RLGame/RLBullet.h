#pragma once
#include "Math/vec2.h"
#include "2D/Collider2D.h"
#include "RLDef.h"
class b2Body;
namespace tzw
{
class Sprite;
class SpriteInstanceInfo;
struct RLBulletInfo
{
	float m_damage = 0.f;
	int m_combatStrengh = 0;
};
struct RLBullet
{
	RLBullet(vec2 Pos, vec2 Velocity, bool isAllyBullet);
	void onCollision(b2Body * self, b2Body * other, b2Contact* contact);

	vec2 m_pos;
	vec2 m_velocity;
	vec2 getPosition();
	int m_typeID = 0;
	SpriteInstanceInfo * m_sprite = nullptr;
	Collider2D m_collider2D;
	float m_lifespan = 1.f;
	float m_t = 0.f;
	bool m_isLiving = true;
	RLBulletInfo m_info;
	b2Body * m_body;
	RLUserDataWrapper m_wrapper;
};
}
