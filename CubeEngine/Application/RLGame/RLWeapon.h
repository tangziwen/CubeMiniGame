#pragma once
#include "Math/vec2.h"

namespace tzw
{
class RLHero;
class RLWeapon
{
public:
	vec2 getShootDir() const;

	void setShootDir(const vec2& shootDir);

	void fire();
	void onTick(float dt);
	void setOwner(RLHero * hero);
	
protected:
	float m_cd = 2.0;
	float m_currTime = 0.0;
	RLHero * m_owner = nullptr;
	vec2 m_shootDir;
};
}
