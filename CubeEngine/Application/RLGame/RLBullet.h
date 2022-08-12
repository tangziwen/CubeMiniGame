#pragma once
#include "Math/vec2.h"
#include "2D/Collider2D.h"
namespace tzw
{
class Sprite;
class SpriteInstanceInfo;
struct RLBulletInfo
{
	float m_damage = 0.f;
};
struct RLBullet
{
	RLBullet();
	void onCollision(Collider2D * self, Collider2D * other);

	vec2 m_pos;
	vec2 m_velocity;
	int m_typeID = 0;
	SpriteInstanceInfo * m_sprite = nullptr;
	Collider2D m_collider2D;
	float m_lifespan = 1.f;
	float m_t = 0.f;
	bool m_isLiving = true;
	RLBulletInfo m_info;
};
}
