#pragma once
#include <string>

#include "Math/vec2.h"

namespace tzw
{
class RLHero;
class RLWeaponData;
class RLWeapon
{
public:
	RLWeapon(int typeID);
	RLWeapon(std::string typeName);
	bool isAutoFiring() const
	{
		return m_isAutoFiring;
	}

	void setIsAutoFiring(bool enable)
	{
		m_isAutoFiring = enable;
	}

	vec2 getShootDir() const;

	void setShootDir(const vec2& shootDir);

	virtual void fireOneRound();
	void fire();
	void onTick(float dt);
	void setOwner(RLHero * hero);
	
protected:
	float m_cd = 0.8f;
	float m_currTime = 0.0;
	int m_currRound = 0;
	RLHero * m_owner = nullptr;
	vec2 m_shootDir;
	bool m_isAutoFiring = false;
	int m_typeID = 0;
	RLWeaponData * m_data;
	bool m_isFiring = false;
};
}
