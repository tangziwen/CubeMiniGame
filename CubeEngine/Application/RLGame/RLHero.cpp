#include "RLHero.h"

#include "RLHeroCollection.h"
#include "RLWorld.h"
#include "RLPlayerState.h"
#include "RLCollectible.h"
#include "RLSpritePool.h"
#include "RLUtility.h"
#include "RLSkills.h"

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
	m_HP = m_heroData->m_maxHealth;
	m_MAXHP = m_heroData->m_maxHealth;
	m_Speed = m_heroData->m_speed;
	if(!m_heroData->m_defaultWeapon.empty())
	{
		equipWeapon(new RLWeapon(m_heroData->m_defaultWeapon));
	}
	
}

RLHero::~RLHero()
{
	//delete m_sprite;
	RLSpritePool::shared()->get()->removeSprite(m_sprite);
	RLSpritePool::shared()->get()->removeSprite(m_hpBar);
	delete m_collider;
	delete m_weapon;
}

void RLHero::setPosition(vec2 pos)
{
	RLUtility::shared()->clampToBorder(pos);
	m_collider->setPos(pos);
}
const vec2& RLHero::getPosition()
{
	return m_collider->getPos();
}

void RLHero::updateGraphics()
{
	vec2 pos = m_collider->getPos();
	if(!m_isInitedGraphics)
	{
		initGraphics();
	}
	if(m_sprite)
	{
		m_sprite->pos = pos;
		m_hpBar->pos = pos + vec2(0, -16);
		m_hpBar->scale = vec2(m_HP / m_MAXHP, 0.125f);
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


	int hpBarType = RLSpritePool::shared()->get()->getOrAddType("RL/Sprites/HpBar.png");
	SpriteInstanceInfo * hpInfo = new SpriteInstanceInfo();
	hpInfo->type = hpBarType;
	m_hpBar = hpInfo;
	m_hpBar->layer = 3;
	m_hpBar->pos = getPosition();
	m_hpBar->scale = vec2(1, 0.125f);
	RLSpritePool::shared()->get()->addTile(m_hpBar);

	m_isInitedGraphics = true;
}

void RLHero::onTick(float dt)
{
	m_container.tick(dt);
	if(m_currSkill)
	{
		m_currSkill->onTick(dt);
		if(m_currSkill->isFinished())
		{
			m_currSkill->raiseFinished();
			delete m_currSkill;
			m_currSkill = nullptr;
		}
	}
	updateGraphics();
	if(m_controller)
	{
		m_controller->tick(dt);
	}
	if(m_weapon && !m_isDash && !m_isDeflect)
	{
		m_weapon->onTick(dt);
	}
	//m_collider->setPos(m_pos);
	if(!m_collider->getParent())
	{
		RLWorld::shared()->getQuadTree()->addCollider(m_collider);
	}
	if(m_isDash)
	{
		//m_dashSpeed += 900.0 * dt;
		//m_dashSpeed = std::clamp(m_dashSpeed, 0.0f, 500.f);//about 0.16s to reach the limit
		m_dashSpeed = 250.0f;
		if(m_dashTimer <= 0.25f)
		{
			m_collider->setIsCollisionEnable(false);
			m_sprite->overLayColor = vec4(0.5, 0.5, 1.0, 1.0f);
		}
		else if(m_dashTimer > 0.25f && m_dashTimer <= 0.5f)
		{
			m_collider->setIsCollisionEnable(true);
			m_sprite->overLayColor = vec4(0.5, 0.5, 1.0, 0.7f);
		}
		if(m_dashTimer > 0.5f)
		{
			m_collider->setIsCollisionEnable(true);
			m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 0.0);
			m_isDash = false;
		}
		m_dashVelocity = m_dashDir * m_dashSpeed * dt;//vec2(1 * m_dashSpeed* dt, 0);
		setPosition(getPosition() + m_dashVelocity );
		m_dashTimer += dt;


	}

	if(m_isDeflect)
	{
		if(m_deflectTimer < 0.2f)
		{
			m_sprite->overLayColor = vec4(255 / 255.f, 138.f / 255.f, 54.f / 255, 1.0);

			//����
			vec2 minV = getPosition() - vec2(32, 32);
			vec2 maxV = getPosition() + vec2(32, 32);
			std::vector<Collider2D*> colliderList;
			RLWorld::shared()->getQuadTree()->getRange(AABB2D(minV, maxV), colliderList);
			for(Collider2D * collider : colliderList)
			{
				//filter enemyBullet
				if(collider->getSourceChannel() == CollisionChannel2D_Bullet
					&& collider->getResponseChannel() == CollisionChannel2D_Player)
				{
					auto bullet = static_cast<RLBullet *> (collider->getUserData().m_userData);
					vec2 diff = bullet->m_pos - getPosition();
					if(diff.length() < 64)
					{
						bullet->m_velocity = bullet->m_velocity * -1;
						bullet->m_t = 0.f;
						bullet->m_sprite->type = 1;
						collider->setResponseChannel(CollisionChannel2D_Entity); 
					}

				}
			
			
			}
		
		}
		else
		{
			m_sprite->overLayColor = vec4(219.f / 255.f, 52.f / 255.f, 0, 1.0);
		
		}

		m_deflectTimer += dt;
	}
	
	if(getIsPlayerControll())
	{
		m_hitImmuneTimer += dt;
		if(m_isHitImmune && m_hitImmuneTimer > 0.35f)
		{
			m_isHitImmune = false;
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

	

	m_HP = std::clamp(m_HP, 0.f, m_MAXHP);
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
					hero->receiveDamage(5);
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
		m_sprite->overLayColor = vec4(219.f / 255.f, 52.f / 255.f, 0, 1.0);
	}
	else
	{
		m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 1.0);
	}

	m_HP -= damage;
	if(m_HP <= 0.f)
	{
		if(getIsPlayerControll())
		{
			RLWorld::shared()->setCurrGameState(RL_GameState::AfterMath);
		}
		else
		{
			RLPlayerState::shared()->addScore(10);
			if(rand() % 10 > 3)
			{
				if(rand() % 10 > 2)
				{
					RLCollectibleMgr::shared()->addCollectible(0, getPosition());
				}
				else
				{
					RLCollectibleMgr::shared()->addCollectible(2, getPosition());
				}

			}
			
		}
	}

}

bool RLHero::isAlive()
{
	return m_HP > 0.f;
}

void RLHero::setController(RLController* controller)
{
	m_controller = controller;
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
	if(m_isDash) return;
	m_dashSpeed = 150.0f;
	m_isDash = true;
	m_dashTimer = 0.0f;
	m_collider->setIsCollisionEnable(false);
	m_sprite->overLayColor = vec4(0.5, 0.5, 1.0, 1.0);
	m_container.trigger(RLEffectGrantType::OnDash);
	if(m_Mana > 30.0f)
	{
		m_Mana = std::max(15.0f, m_Mana - 10.0f);
	}
	else
	{
		m_HP = std::max(1.f, m_HP - 10.0f);
	
	}

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
	if(m_isDeflect) return;
	m_moveDir = dir;
	m_isMoving = true;
	float targetSpeed = m_Speed+ m_container.modifier("MovementSpeed_Add");
	if(m_weapon)
	{
		if(!getIsPlayerControll() && m_weapon->isFiring())
		{
			targetSpeed *= 0.5f;
		}
	}
	setPosition(getPosition() + dir * delta * targetSpeed);
}

void RLHero::applyEffect(std::string name)
{
	RLEffectInstance * instance =  RLEffectMgr::shared()->getInstance(this, name, &m_container);
	m_container.addEffectInstance(instance);
}

void RLHero::applyEffect(RLEffect* effect)
{
	RLEffectInstance * instance =  RLEffectMgr::shared()->getInstance(this, effect, &m_container);
	m_container.addEffectInstance(instance);
}

bool RLHero::startDeflect()
{
	m_isDeflect = true;
	m_deflectTimer = 0.f;
	return true;
}

void RLHero::endDeflect()
{
	m_sprite->overLayColor = vec4(1, 1, 1, 0.0);
	m_isDeflect = false;
	m_deflectTimer = 0.f;
}

RLHeroData* RLHero::getHeroData()
{
	return m_heroData;
}

RLSkillBase* RLHero::playSkill()
{
	m_currSkill = new RLSkillCharge(this);
	m_currSkill->onEnter();
	return m_currSkill;
}

RLSkillBase* RLHero::playSkillToTarget(RLHero* hero)
{
	m_currSkill = new RLSkillCharge(this);
	m_currSkill->getBlackBoard().writeData("SkillTarget", hero);
	m_currSkill->onEnter();
	return m_currSkill;
}



}
