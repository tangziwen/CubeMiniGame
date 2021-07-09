#pragma once
#include "FPGunData.h"
#include "EngineSrc/3D/Model/Model.h"
namespace tzw
{
class FPGun
{
public:
	FPGun(FPGunData * gunData);
	Model * getModel() const {return m_model;}
	bool getIsADS() const {return m_isAds;}
	void setIsADS(bool isADS, bool isNeedTransient);
	void toggleADS(bool isNeedTransient);
	void tick(bool isMoving, float dt);
protected:
	FPGunData * m_data {nullptr};
	Model * m_model {nullptr};
	bool m_isAds {false};
	float m_shakeTime {0.f};
};



}
