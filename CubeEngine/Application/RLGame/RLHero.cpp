#include "RLHero.h"

#include "RLHeroCollection.h"
#include "RLWorld.h"
#include "RLPlayerState.h"
#include "RLCollectible.h"
#include "RLSpritePool.h"
namespace tzw
{
RLHero::RLHero(int idType)
	:m_id(idType)
{
	m_collider = new Collider2D(16, vec2(0, 0));
	m_collider->m_cb = [this](Collider2D* self, Collider2D* other)
	{
		onCollision(self, other);
	};
	m_collider->setUserData(UserDataWrapper(this, RL_OBJECT_TYPE_MONSTER));
	m_heroData = RLHeroCollection::shared()->getHeroData(idType);
	m_hp = m_heroData->m_maxHealth;
}

RLHero::~RLHero()
{
	//delete m_sprite;
	RLSpritePool::shared()->get()->removeSprite(m_sprite);
	delete m_collider;
	delete m_weapon;
}

void RLHero::setPosition(vec2 pos)
{
	m_collider->setPos(pos);
	m_pos = pos;
}
const vec2& RLHero::getPosition()
{
	return m_pos;
}

void RLHero::updateGraphics()
{
	m_pos = m_collider->getPos();
	if(!m_isInitedGraphics)
	{
		initGraphics();
	}
	if(m_sprite)
	{
		m_sprite->pos = m_pos;
	}
}

void RLHero::initGraphics()
{
	int spriteType = RLSpritePool::shared()->get()->getOrAddType(m_heroData->m_sprite);
	SpriteInstanceInfo * info = new SpriteInstanceInfo();
	info->type = spriteType;
	m_sprite = info;
	m_sprite->layer = 2;
	m_sprite->pos = getPosition();
	RLSpritePool::shared()->get()->addTile(m_sprite);
	m_isInitedGraphics = true;
}

void RLHero::onTick(float dt)
{

	updateGraphics();
	if(m_weapon)
	{
		m_weapon->onTick(dt);
	}
	m_collider->setPos(m_pos);
	if(!m_collider->getParent())
	{
		RLWorld::shared()->getQuadTree()->addCollider(m_collider);
	}
	if(m_isDash)
	{
		//m_dashSpeed += 900.0 * dt;
		//m_dashSpeed = std::clamp(m_dashSpeed, 0.0f, 500.f);//about 0.16s to reach the limit
		m_dashSpeed = 450.0f;
		m_dashVelocity = m_dashDir * m_dashSpeed * dt;//vec2(1 * m_dashSpeed* dt, 0);
		setPosition(getPosition() + m_dashVelocity );
		m_dashTimer += dt;
		if(m_dashTimer >= 0.3f)
		{
			m_isDash = false;
		}
	}
	if(m_controller)
	{
		m_controller->tick(dt);
	}

	
	if(getIsPlayerControll())
	{
		m_hitImmuneTimer += dt;
		if(m_isHitImmune && m_hitImmuneTimer > 0.35f)
		{
			m_isHitImmune = false;
			m_collider->setIsCollisionEnable(true);
			m_hitImmuneTimer = 0.f;
		}
	}

	float hitEffectThreshold = 0.08f;
	if (getIsPlayerControll())
	{
		hitEffectThreshold = 0.35f;
	}
	m_hitEffectTimer += dt;
	if(m_triggerHitEffect && m_hitEffectTimer > hitEffectThreshold)
	{
		m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 0.0);
		m_triggerHitEffect = false;
		m_hitEffectTimer = 0.f;
	}

	m_isMoving = false;
}

void RLHero::equipWeapon(RLWeapon* weapon)
{
	m_weapon = weapon;
	m_weapon->setOwner(this);
}

RLWeapon * RLHero::getWeapon()
{
	return m_weapon;
}

Collider2D* RLHero::getCollider2D()
{
	return m_collider;
}

void RLHero::onCollision(Collider2D* self, Collider2D* other)
{
	if(!getIsPlayerControll())
	{
		if(other->getUserData().m_userData)
		{
			
			switch(other->getUserData().m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				
				RLHero * hero = reinterpret_cast<RLHero *>(other->getUserData().m_userData);
				if(hero->getIsPlayerControll())
				{
					hero->receiveDamage(5.f);
				}
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

void RLHero::receiveDamage(float damage)
{
	if(m_isHitImmune) return;
	m_triggerHitEffect = true;
	m_hitEffectTimer = 0.f;
	if(getIsPlayerControll())
	{
		m_isHitImmune = true;
		m_collider->setIsCollisionEnable(false);
		m_sprite->overLayColor = vec4(1.0, 1.0, 0.5, 1.0);
	}
	else
	{
		m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	return;
	m_hp -= damage;
	if(m_hp <= 0.f)
	{
		if(getIsPlayerControll())
		{
			RLWorld::shared()->setCurrGameState(RL_GameState::AfterMath);
		}
		else
		{
			RLPlayerState::shared()->addScore(10);
			RLCollectibleMgr::shared()->addCollectible(0, getPosition());
		}
	}

}

bool RLHero::isAlive()
{
	return m_hp > 0.f;
}

void RLHero::setController(RLController* controller)
{
	m_controller = controller;
}

float RLHero::getHP()
{
	return m_hp;
}

void RLHero::onPossessed()
{
	if(getIsPlayerControll())
	{
		m_collider->setSourceChannel(CollisionChannel2D_Player);
		m_collider->setResponseChannel(CollisionChannel2D_Entity | CollisionChannel2D_Player);
		m_collider->setFriction(10);
		m_collider->setMaxSpeed(120);
	}
	else
	{
		m_collider->setSourceChannel(CollisionChannel2D_Entity);
		m_collider->setResponseChannel(CollisionChannel2D_Player);
	}
}

void RLHero::doDash()
{
	m_dashSpeed = 150.0f;
	m_isDash = true;
	m_dashTimer = 0.0f;
	if (m_isMoving)
	{
		m_dashDir = m_moveDir;
	}
	else
	{
		m_dashDir = m_weapon->getShootDir();
	}
}

void RLHero::doMove(vec2 dir, float delta)
{
	if(m_isDash) return;
	m_moveDir = dir;
	m_isMoving = true;
	setPosition(getPosition() + dir * delta);
}

}
