#include "RLHero.h"

#include "RLHeroCollection.h"
#include "RLWorld.h"
#include "RLPlayerState.h"
#include "RLCollectible.h"
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
	delete m_sprite;
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
		m_sprite->setPos2D(m_pos);
	}
}

void RLHero::initGraphics()
{
	m_sprite = Sprite::create(m_heroData->m_sprite);
	m_sprite->setLocalPiority(2);
	RLWorld::shared()->getRootNode()->addChild(m_sprite);
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
	if(m_controller)
	{
		m_controller->tick(dt);
	}

	if(getIsPlayerControll())
	{
		m_hitTimer += dt;
		if(m_hitTimer > 0.25f)
		{
			m_isHitImmune = false;
			m_collider->setIsCollisionEnable(true);
			m_hitTimer = 0.f;
		}
	}

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
	m_isHitImmune = true;
	m_collider->setIsCollisionEnable(false);
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
	}
	else
	{
		m_collider->setSourceChannel(CollisionChannel2D_Entity);
		m_collider->setResponseChannel(CollisionChannel2D_Player);
	}
}

}
