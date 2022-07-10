#include "RLWeapon.h"

#include "RLBulletPool.h"
#include "RLHero.h"

namespace tzw
{
void RLWeapon::fire()
{
	RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), vec2(100, 0));
}
void RLWeapon::onTick(float dt)
{
	m_currTime += dt;
	if(m_currTime > m_cd)
	{
		fire();
		m_currTime = 0.f;
	}
}
void RLWeapon::setOwner(RLHero* hero)
{
	m_owner = hero;
}
}
