#include "RLWeapon.h"

#include "RLBulletPool.h"
#include "RLHero.h"
#include "RLWeaponCollection.h"
#include "Utility/math/TbaseMath.h"
namespace tzw
{
RLWeapon::RLWeapon(int typeID):m_typeID(typeID)
{
	m_data = RLWeaponCollection::shared()->getWeaponData(typeID);
	m_currRound = m_data->m_fireRound;
}
RLWeapon::RLWeapon(std::string typeName)
{
	m_data = RLWeaponCollection::shared()->getWeaponData(typeName);
	m_typeID = m_data->m_id;
	m_currRound = m_data->m_fireRound;
	m_splitMount = m_data->m_splitNum;
	m_splitAngle = m_data->m_splitAngle;
}
vec2 RLWeapon::getShootDir() const
{
	return m_shootDir;
}

void RLWeapon::setShootDir(const vec2& shootDir)
{
	m_shootDir = shootDir;
}

void RLWeapon::fireOneRound()
{

	if(m_splitMount == 0)
	{
		RLBulletInfo info;
		info.m_damage = m_data->m_damage + m_owner->getDamage();
		RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), m_shootDir * m_data->m_bulletSpeed, m_owner->getIsPlayerControll(), info);
	}
	else
	{
		float angleDiff = (m_splitAngle *2.f) / (m_splitMount);
		Matrix44 mat;
		Quaternion quat;
		for(int i =0 ; i <= m_splitMount; i++)
		{
			quat.fromEulerAngle(vec3(0, 0, -m_splitAngle + angleDiff * i));
			mat.setRotation(quat); 
			vec4 newDir = mat * vec4(m_shootDir.x, m_shootDir.y, 0.0, 0.0);
			vec2 shootDir = vec2(newDir.x, newDir.y);
			shootDir = shootDir.normalized();

			RLBulletInfo info;
			info.m_damage = m_data->m_damage + m_owner->getDamage();
			RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), shootDir * m_data->m_bulletSpeed, m_owner->getIsPlayerControll(), info);
		}
	
	}

}

void RLWeapon::fire()
{
	m_isFiring = true;
}

void RLWeapon::onTick(float dt)
{

	m_currTime += dt;
	if(m_currRound <= 0)//now is reloading
	{
		m_isFiring = false;
		if(m_currTime > m_data->m_cd)
		{
			m_currRound = m_data->m_fireRound;
			m_currTime = m_data->m_fireRate;//let the weapon immediately shoot
		}
	}
	else
	{
		if((m_isFiring || m_isAutoFiring) && m_currTime >= m_data->m_fireRate)
		{
			fireOneRound();
			m_currTime = 0.f;
			m_currRound -= 1;
		}
	}
}
void RLWeapon::setOwner(RLHero* hero)
{
	m_owner = hero;
}
}
