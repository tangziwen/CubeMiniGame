#include "RLHero.h"
#include "RLWorld.h"
#include "RLPlayerState.h"
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
}

RLHero::~RLHero()
{
	delete m_sprite;
	delete m_collider;
	delete m_weapon;
}

void RLHero::setPosition(vec2 pos)
{
	m_pos = pos;
}
const vec2& RLHero::getPosition()
{
	return m_pos;
}

void RLHero::updateGraphics()
{
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
	if(m_id == 0)
	{
		m_sprite = Sprite::create("PTM/army.png");

	}
	else
	{
		m_sprite = Sprite::create("RL/zombie.png");
	}
	
	m_sprite->setLocalPiority(2);
	RLWorld::shared()->getRootNode()->addChild(m_sprite);
	m_isInitedGraphics = true;
	
}

void RLHero::onTick(float dt)
{
	if(m_controller)
	{
		m_controller->tick(dt);
	}
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
	if(!m_isHitImmune)
	{
		RLWorld::shared()->getQuadTree()->checkCollision(m_collider);
	}
	else
	{
		m_hitTimer += dt;
		if(m_hitTimer > 0.25f)
		{
			m_isHitImmune = false;
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
	if(getIsPlayerControll() && !m_isHitImmune)
	{
		receiveDamage(15.0f);
		m_isHitImmune = true;
	}
}

void RLHero::receiveDamage(float damage)
{
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

}
