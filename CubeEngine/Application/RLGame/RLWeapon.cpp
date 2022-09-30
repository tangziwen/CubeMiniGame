#include "RLWeapon.h"

#include "RLBulletPool.h"
#include "RLHero.h"
#include "RLWeaponCollection.h"
#include "Utility/math/TbaseMath.h"
namespace tzw
{
//Aim Policy

RLAimPolicyType RLAimPolicy::getAimPolicyType()
{
	return RLAimPolicyType::DirectShoot;
}

RLAimPolicyType RLAimPolicySpray::getAimPolicyType()
{
	return RLAimPolicyType::Spray;
}

void RLAimPolicySpray::reset()
{
	m_timer = 0.f;
}

void RLAimPolicySpray::tick(float dt)
{
	m_timer += ((2.0f * 3.1416f) / m_SprayTime) * dt;
}


RLAimPolicyType RLAimPolicyCircle::getAimPolicyType()
{
	return RLAimPolicyType::Circle;
}

void RLAimPolicyCircle::reset()
{
	m_timer = 0.f;
}

void RLAimPolicyCircle::tick(float dt)
{
	m_timer += ((360.f) / m_circleTime) * dt;
}


RLAimPolicyType RLAimPolicySprayDiscrete::getAimPolicyType()
{
	return RLAimPolicyType::SprayDiscrete;
}

void RLAimPolicySprayDiscrete::reset()
{
	m_currAngle = -m_SprayAngle;
}

void RLAimPolicySprayDiscrete::tick(float dt)
{
}

float RLAimPolicySprayDiscrete::onBulletShoot()
{
	m_bulletCount += 1;
	if(m_bulletCount >= m_shiftBulletCount)
	{
		m_currAngle += (m_SprayAngle * 2.0) / (m_splitCount- 1);
		m_bulletCount = 0;
		if(m_currAngle > m_SprayAngle + 0.01)
		{
			m_currAngle = -m_SprayAngle;
		}
		return -0.3f;
	}
	return 0;
}


RLWeapon::RLWeapon(int typeID):m_typeID(typeID)
{
	m_data = RLWeaponCollection::shared()->getWeaponData(typeID);
	init();
}
RLWeapon::RLWeapon(std::string typeName)
{
	m_data = RLWeaponCollection::shared()->getWeaponData(typeName);
	m_typeID = m_data->m_id;
	init();

}

void RLWeapon::setFullAmmo()
{
	m_currRound = m_data->m_fireRound;
}

void RLWeapon::init()
{

	m_currRound = 0;
	m_splitMount = m_data->m_splitNum;
	m_splitAngle = m_data->m_splitAngle;

	m_currTime = 0.f;
	if(m_data->AimPolicyType == "DirectShoot")
	{
		setAimPolicy(new RLAimPolicy());
	}
	else if(m_data->AimPolicyType == "Spray")
	{
		setAimPolicy(new RLAimPolicySpray());
		auto policy = static_cast<RLAimPolicySpray * > (m_aimPolicy);
		policy->m_SprayAngle = m_data->m_sprayAngle;
		policy->m_SprayTime = m_data->m_sprayTime;
	}
	else if(m_data->AimPolicyType == "Circle")
	{
		setAimPolicy(new RLAimPolicyCircle());
		auto policy = static_cast<RLAimPolicyCircle * > (m_aimPolicy);
		policy->m_circleTime = m_data->m_circleTime;
	}
	else if(m_data->AimPolicyType == "SprayDiscrete")
	{
		setAimPolicy(new RLAimPolicySprayDiscrete());
		auto policy = static_cast<RLAimPolicySprayDiscrete * > (m_aimPolicy);
		policy->m_SprayAngle = m_data->m_sprayAngle;
		policy->m_shiftBulletCount = m_data->m_shiftBulletCount;
		policy->m_splitCount = m_data->m_splitAngleCount;
	}
	m_isCanChangeDirWhilstShoot = m_data->m_isCanChangeDirWhilstShoot;
}
vec2 RLWeapon::getShootDir() const
{
	return m_shootDir;
}

int RLWeapon::getTotalRound()
 {return m_data->m_fireRound;}

 void RLWeapon::reload()
 {
	 if(m_currRound< m_data->m_fireRound)
	 {
		m_isReloading = true;
		m_currTime = 0.f;
	 }
 }
void RLWeapon::setShootDir(const vec2& shootDir)
{
	if(m_isCanChangeDirWhilstShoot)
	{
		m_shootDir = shootDir;
	}
	else
	{
		if(m_currRound <= 0)//only can change direction whilst reloading.
		{
			m_shootDir = shootDir;
		}
	}
	
}

void RLWeapon::fireOneRound()
{
	if(m_currRound <= 0)
	{
		reload();
		return;
	}
	vec2 currShootDir;
	
	switch(m_aimPolicy->getAimPolicyType())
	{
	case RLAimPolicyType::DirectShoot:
		currShootDir = m_shootDir;
		break;
	case RLAimPolicyType::Spray:
	{
		auto spray = static_cast<RLAimPolicySpray*>(m_aimPolicy);
		Matrix44 matDir;
		Quaternion quatDir;
		quatDir.fromEulerAngle(vec3(0, 0, 45.0f * cosf(spray->m_timer)));
		matDir.setRotation(quatDir); 
		vec4 newDirForShoot = matDir * vec4(m_shootDir.x, m_shootDir.y, 0.0, 0.0);
		currShootDir = vec2(newDirForShoot.x, newDirForShoot.y);
		currShootDir = currShootDir.normalized();
	}
	break;
	case RLAimPolicyType::SprayDiscrete:
	{
		auto spray = static_cast<RLAimPolicySprayDiscrete*>(m_aimPolicy);
		Matrix44 matDir;
		Quaternion quatDir;
		quatDir.fromEulerAngle(vec3(0, 0, spray->m_currAngle));
		matDir.setRotation(quatDir); 
		vec4 newDirForShoot = matDir * vec4(m_shootDir.x, m_shootDir.y, 0.0, 0.0);
		currShootDir = vec2(newDirForShoot.x, newDirForShoot.y);
		currShootDir = currShootDir.normalized();
	}
	break;
	case RLAimPolicyType::Circle:
	{
		auto circle = static_cast<RLAimPolicyCircle*>(m_aimPolicy);
		Matrix44 matDir;
		Quaternion quatDir;
		quatDir.fromEulerAngle(vec3(0, 0, circle->m_timer));
		matDir.setRotation(quatDir); 
		vec4 newDirForShoot = matDir * vec4(m_shootDir.x, m_shootDir.y, 0.0, 0.0);
		currShootDir = vec2(newDirForShoot.x, newDirForShoot.y);
		currShootDir = currShootDir.normalized();
	}
	break;
	}


	if(m_splitMount == 0)
	{
		RLBulletInfo info;
		info.m_damage = m_data->m_damage + m_owner->getDamage();
		info.m_combatStrengh = m_owner->getCombatStrengh();
		RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), currShootDir * m_data->m_bulletSpeed, m_owner->getIsPlayerControll(), info);
	}
	else
	{
		if(m_splitAngle < 0.0f)//full circle range
		{
			float angleDiff = (360.0) / (m_splitMount + 1);
			Matrix44 mat;
			Quaternion quat;
			for(int i =0 ; i <= m_splitMount; i++)
			{
				quat.fromEulerAngle(vec3(0, 0, angleDiff * i));
				mat.setRotation(quat); 
				vec4 newDir = mat * vec4(currShootDir.x, currShootDir.y, 0.0, 0.0);
				vec2 shootDir = vec2(newDir.x, newDir.y);
				shootDir = shootDir.normalized();

				RLBulletInfo info;
				info.m_damage = m_data->m_damage + m_owner->getDamage();
				info.m_combatStrengh = m_owner->getCombatStrengh();
				RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), shootDir * m_data->m_bulletSpeed, m_owner->getIsPlayerControll(), info);
			}
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
				vec4 newDir = mat * vec4(currShootDir.x, currShootDir.y, 0.0, 0.0);
				vec2 shootDir = vec2(newDir.x, newDir.y);
				shootDir = shootDir.normalized();

				RLBulletInfo info;
				info.m_damage = m_data->m_damage + m_owner->getDamage();
				info.m_combatStrengh = m_owner->getCombatStrengh();
				RLBulletPool::shared()->spawnBullet(1, m_owner->getPosition(), shootDir * m_data->m_bulletSpeed, m_owner->getIsPlayerControll(), info);
			}
		}

	
	}
	m_currTime = 0.f;
	m_currRound -= 1;
	m_currTime += m_aimPolicy->onBulletShoot();
}

void RLWeapon::fire()
{
	m_isFiring = true;
}

void RLWeapon::onTick(float dt)
{
	if(m_aimPolicy)
		m_aimPolicy->tick(dt);
	m_aimTimer += dt;
	m_currTime += dt;
	if(m_isReloading)//now is reloading
	{
		m_isFiring = false;
		if(m_currTime > m_data->m_cd)
		{
			m_currRound = m_data->m_fireRound;
			m_currTime = m_data->m_fireRate;//let the weapon immediately shoot
			m_isReloading = false;
			if(m_aimPolicy)
				m_aimPolicy->reset();
		}
	}
	else
	{
		if((m_isFiring || m_isAutoFiring) && m_currTime >= m_data->m_fireRate)
		{
			fireOneRound();

		}
	}
}
void RLWeapon::setOwner(RLHero* hero)
{
	m_owner = hero;

}

void RLWeapon::setAimPolicy(RLAimPolicy* policy)
{
	m_aimPolicy = policy;
}

bool RLWeapon::isFiring()
{
	return m_currRound > 0 && m_isAutoFiring;
}





}
