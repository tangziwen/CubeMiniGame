#include "FPGun.h"

#include "CubeGame/BulletMgr.h"
#include "CubeGame/GameUISystem.h"
#include "Lighting/PointLight.h"
#include "Scene/SceneMgr.h"
#include "AudioSystem/AudioSystem.h"
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
    auto pointLight = new PointLight();
    pointLight->setRadius(5);
    pointLight->setLightColor(vec3(5, 2.5, 0));
    pointLight->setPos(vec3(-33.408, 0, 0));
	pointLight->setIsVisible(false);
	m_pointLight = pointLight;
	g_GetCurrScene()->addNode(pointLight);
	m_fireSound = AudioSystem::shared()->createSound("Sound/m4a1.wav");
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
	if(m_pointLight->getIsVisible())
	{
		m_flashTime += dt;
		if(m_flashTime > 0.035)
		{
			m_pointLight->setIsVisible(false);
			m_flashTime = 0.0;
		}
	}

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
	vec3 gunPointPos = m_model->getTransform().transformVec3(vec3(-33.408,0, 0));
	
	auto bullet = BulletMgr::shared()->fire(nullptr,cam->getWorldPos() ,gunPointPos, cam->getForward(), 15, BulletType::HitScanTracer);
	m_pointLight->setIsVisible(true);
	m_pointLight->setPos(cam->getWorldPos() + cam->getForward() * 0.15);
	m_flashTime = 0.0;
	auto event = m_fireSound->playWithOutCare();
	event.setVolume(1.2f);
}
}

