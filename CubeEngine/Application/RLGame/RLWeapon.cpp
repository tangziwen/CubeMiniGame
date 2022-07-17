#include "RLWeapon.h"

#include "RLBulletPool.h"
#include "RLHero.h"

namespace tzw
{
vec2 RLWeapon::getShootDir() const
{
	return m_shootDir;
}

void RLWeapon::setShootDir(const vec2& shootDir)
{
	m_shootDir = shootDir;
}

void RLWeapon::fire()
{
	RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), m_shootDir * 200.0f);
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
