#include "FPGun.h"

#include "CubeGame/BulletMgr.h"
#include "CubeGame/GameUISystem.h"
#include "Scene/SceneMgr.h"

namespace tzw
{

FPGun::FPGun(FPGunData * gunData):
	m_data(gunData)
{
	m_model = Model::create(m_data->m_filePath);
	m_model->setPos(0.12,0.6, -0.22);
	m_model->setRotateE(m_data->m_rotateE);
	m_model->setScale(vec3(m_data->m_scale, m_data->m_scale, m_data->m_scale));
	m_model->setIsAccpectOcTtree(false);
}

void FPGun::setIsADS(bool isADS, bool isNeedTransient)
{
	if(isADS && !m_data->m_isAllowADS)//not allowed aim down sight
		return;
	m_isAds = isADS;
	if(m_isAds)
	{
		GameUISystem::shared()->setIsNeedShowCrossHair(false);
	}
	else
	{
		GameUISystem::shared()->setIsNeedShowCrossHair(true);
	}
}

void FPGun::toggleADS(bool isNeedTransient)
{
	setIsADS(!m_isAds, isNeedTransient);
}

void FPGun::tick(bool isMoving, float dt)
{
	float offset = 0.002;
	if(m_isAds)
	{
		offset = 0.0005;
	}
	float freq = 1.2;
	if (isMoving)
	{
		if(m_isAds)
		{
			offset = 0.0008;
		}
		else
		{
			offset = 0.006;
		}
		freq = 8;
	}
	m_shakeTime += freq * dt;
	if(m_isAds)
	{
		m_model->setPos(vec3(m_data->m_adsPos.x, m_data->m_adsPos.y + sinf(m_shakeTime) * offset, m_data->m_adsPos.z));
	}
	else
	{
		m_model->setPos(vec3(m_data->m_hipPos.x, m_data->m_hipPos.y + sinf(m_shakeTime) * offset, m_data->m_hipPos.z));
	}
}

void FPGun::shoot()
{
	auto cam = g_GetCurrScene()->defaultCamera();
	auto mdata = cam->getTransform().data();
	auto bullet = BulletMgr::shared()->fire(nullptr,cam->getWorldPos() ,cam->getWorldPos() + vec3(mdata[0], mdata[1], mdata[2]) * 0.25, cam->getForward(), 15, BulletType::HitScanTracer);
}
}
