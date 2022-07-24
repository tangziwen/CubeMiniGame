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
	bool isEnable() const
	{
		return m_enable;
	}

	void setEnable(bool enable)
	{
		m_enable = enable;
	}

	vec2 getShootDir() const;

	void setShootDir(const vec2& shootDir);

	virtual void fire();
	void onTick(float dt);
	void setOwner(RLHero * hero);
	
protected:
	float m_cd = 0.8f;
	float m_currTime = 0.0;
	int m_currRound = 0;
	RLHero * m_owner = nullptr;
	vec2 m_shootDir;
	bool m_enable = true;
	int m_typeID = 0;
	RLWeaponData * m_data;
};
}
