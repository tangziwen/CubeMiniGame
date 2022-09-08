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
		UserDataWrapper wrapper;
		wrapper.m_userData = this;
		wrapper.m_tag = RL_OBJECT_TYPE_BULLET;
		m_collider2D.setUserData(wrapper);
	}
	void RLBullet::onCollision(Collider2D* self, Collider2D* other)
	{
		
		if(!m_isLiving) return;
		if(other->getUserData().m_userData)
		{
			
			switch(other->getUserData().m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				RLHero * hero = reinterpret_cast<RLHero *>(other->getUserData().m_userData);

				hero->receiveDamage(m_info.m_damage);
				m_isLiving = false;
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
