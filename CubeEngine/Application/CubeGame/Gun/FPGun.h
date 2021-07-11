#pragma once
#include "FPGunData.h"
#include "EngineSrc/3D/Model/Model.h"
namespace tzw
{
class Audio;
class PointLight;
class FPGun
{
public:
	FPGun(FPGunData * gunData);
	Model * getModel() const {return m_model;}
	bool getIsADS() const {return m_isAds;}
	void setIsADS(bool isADS, bool isNeedTransient);
	void toggleADS(bool isNeedTransient);
	void tick(bool isMoving, float dt);
	void shoot();

protected:
	FPGunData * m_data {nullptr};
	Model * m_model {nullptr};
	bool m_isAds {false};
	PointLight * m_pointLight {nullptr};
	float m_shakeTime {0.f};
	float m_flashTime {0.f};
	Audio * m_fireSound {nullptr};
};



}
