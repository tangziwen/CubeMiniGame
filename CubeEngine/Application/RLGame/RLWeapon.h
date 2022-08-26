#pragma once
#include <string>

#include "Math/vec2.h"

namespace tzw
{
class RLHero;
class RLWeaponData;

enum class RLAimPolicyType
{
	DirectShoot,
	Spray,
	SprayDiscrete,
	Circle,

};
struct RLAimPolicy
{
	virtual RLAimPolicyType getAimPolicyType();
	virtual void reset(){};
	virtual void tick(float dt) {};
	virtual float onBulletShoot() {return 0.f;};
};

struct RLAimPolicySpray : public RLAimPolicy
{
	RLAimPolicyType getAimPolicyType() override;

	void reset() override;
	void tick(float dt) override;
	float m_SprayAngle = 60.0f;
	float m_timer = 0.f;
	float m_SprayTime = 2.0f;
};




struct RLAimPolicyCircle : public RLAimPolicy
{
	RLAimPolicyType getAimPolicyType() override;

	void reset() override;
	void tick(float dt) override;
	float m_timer = 0.f;
	float m_circleTime = 1.5f;
};

struct RLAimPolicySprayDiscrete : public RLAimPolicy
{
	RLAimPolicyType getAimPolicyType() override;

	void reset() override;
	void tick(float dt) override;
	virtual float onBulletShoot() override;
	float m_SprayAngle = 60.0f;

	int m_shiftBulletCount = 3;
	float m_currAngle = 0.f;
	int m_splitCount = 3;
	int m_bulletCount = 0;
};

class RLWeapon
{
public:
	RLWeapon(int typeID);
	RLWeapon(std::string typeName);
	void init();
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
	void setAimPolicy(RLAimPolicy * policy);
	bool isFiring();
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
	int m_splitMount = 0;
	float m_splitAngle = 45.0f;
	float m_aimTimer;
	RLAimPolicy * m_aimPolicy = nullptr;
	bool m_isCanChangeDirWhilstShoot = true;
};
}
