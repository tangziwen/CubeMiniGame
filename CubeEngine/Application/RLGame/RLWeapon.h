#pragma once
namespace tzw
{
class RLHero;
class RLWeapon
{
public:
	void fire();
	void onTick(float dt);
	void setOwner(RLHero * hero);
protected:
	float m_cd = 2.0;
	float m_currTime = 0.0;
	RLHero * m_owner = nullptr;
};
}
