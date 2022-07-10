#include "RLHero.h"
#include "RLWorld.h"
namespace tzw
{
RLHero::RLHero(int idType)
	:m_id(idType)
{
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
	m_sprite = Sprite::create("PTM/army.png");
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
}

void RLHero::equipWeapon(RLWeapon* weapon)
{
	m_weapon = weapon;
	m_weapon->setOwner(this);
}

}
