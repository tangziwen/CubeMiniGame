#include "RLBullet.h"
#include "RLHero.h"
#include "RLWorld.h"
namespace tzw
{
	RLBullet::RLBullet()
	{
		m_collider2D.m_cb = [this](Collider2D* self, Collider2D*other)
		{
			onCollision(self, other);
		};
	}
	void RLBullet::onCollision(Collider2D* self, Collider2D* other)
	{
		m_isLiving = false;
		if(other->getUserData().m_userData)
		{
			
			switch(other->getUserData().m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				RLHero * hero = reinterpret_cast<RLHero *>(other->getUserData().m_userData);
				hero->receiveDamage(100.f);
			}
				break;
			default:
			{
			
			
			}
			break;
			}
		}
	}
}
