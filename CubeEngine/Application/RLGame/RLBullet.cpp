#include "RLBullet.h"
#include "RLHero.h"
#include "RLWorld.h"
#include "box2d.h"
#include "RLSFX.h"
namespace tzw
{
	RLBullet::RLBullet(vec2 Pos, vec2 Velocity, bool isAllyBullet)
	{
		m_wrapper.m_cb = [this](b2Body* self, b2Body*other, b2Contact* contact)
		{
			onCollision(self, other, contact);
		};

		m_wrapper.m_userData = this;
		m_wrapper.m_tag = RL_OBJECT_TYPE_BULLET;
	


		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(Pos.x / 32.f, Pos.y / 32.f);
		m_body = RLWorld::shared()->getB2DWorld() ->CreateBody(&bodyDef);
		m_body->GetUserData().pointer = (uintptr_t)&m_wrapper;
		b2CircleShape dynamicSphere;
		dynamicSphere.m_radius = 0.25f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicSphere;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 1.0f;
		fixtureDef.restitution = 0.7;
		if(isAllyBullet)
		{
			fixtureDef.filter.categoryBits = RL_PLAYER_BULLET;
			fixtureDef.filter.maskBits = RL_OBSTACLE | RL_ENEMY;
		}
		else
		{
			fixtureDef.filter.categoryBits = RL_ENEMY_BULLET;
			fixtureDef.filter.maskBits = RL_OBSTACLE | RL_PLAYER;
		}

		m_body->CreateFixture(&fixtureDef);
		m_body->SetBullet(true);

		m_body->SetLinearVelocity(b2Vec2(Velocity.x/ 32.f, Velocity.y / 32.f));

	}
	void RLBullet::onCollision(b2Body * self, b2Body * other, b2Contact* contact)
	{
		contact->SetEnabled(false);
		if(!m_isLiving) return;
		m_isLiving = false;
		RLSFXSpec spec = {"RL/Explosive.png", 0.1f};
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		b2Vec2 hitPoint;
		hitPoint = worldManifold.points[0];// + worldManifold.points[1];
		//hitPoint *= 0.5f;
		RLSFXMgr::shared()->addSFX(vec2(hitPoint.x * 32.f, hitPoint.y * 32.f), spec);
		if(other->GetUserData().pointer)
		{
			RLUserDataWrapper * data = reinterpret_cast<RLUserDataWrapper * >(other->GetUserData().pointer);
			switch(data->m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				RLHero * hero = reinterpret_cast<RLHero *>(data->m_userData);
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
