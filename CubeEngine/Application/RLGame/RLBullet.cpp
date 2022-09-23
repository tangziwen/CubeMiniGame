#include "RLBullet.h"
#include "RLHero.h"
#include "RLWorld.h"
#include "box2d.h"
namespace tzw
{
	RLBullet::RLBullet(vec2 Pos, vec2 Velocity)
	{
		m_collider2D.m_cb = [this](Collider2D* self, Collider2D*other)
		{
			onCollision(self, other);
		};
		UserDataWrapper wrapper;
		wrapper.m_userData = this;
		wrapper.m_tag = RL_OBJECT_TYPE_BULLET;
		m_collider2D.setUserData(wrapper);


		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(Pos.x / 32.f, Pos.y / 32.f);
		m_body = RLWorld::shared()->getB2DWorld() ->CreateBody(&bodyDef);

		b2CircleShape dynamicSphere;
		dynamicSphere.m_radius = 0.25f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicSphere;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 1.0f;
		fixtureDef.restitution = 0.7;
		fixtureDef.filter.categoryBits = RL_PLAYER_BULLET;
		fixtureDef.filter.maskBits = RL_OBSTACLE | RL_ENEMY;
		m_body->CreateFixture(&fixtureDef);
		m_body->SetBullet(true);

		m_body->ApplyLinearImpulseToCenter(b2Vec2(Velocity.x/ 32.f, Velocity.y / 32.f), true);

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
	vec2 RLBullet::getPosition()
	{
		b2Vec2 pos = m_body->GetPosition();
		return vec2(pos.x * 32.f, pos.y * 32.f);
	}
}
