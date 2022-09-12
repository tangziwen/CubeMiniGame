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
				int combatStrengthDiff = m_info.m_combatStrengh - hero->getCombatStrengh();
				float ratio = 1.f;
				if(combatStrengthDiff != 0)
				{
					if(combatStrengthDiff > 0)
					{
						ratio += 0.1f * combatStrengthDiff;
					}
					else
					{
						ratio -= 0.1f * abs(combatStrengthDiff);
					}
					ratio = std::clamp(ratio, 0.1f, 2.0f);
				}
				
				hero->receiveDamage(m_info.m_damage * ratio);
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
