#include "Sky.h"
#include "../Scene/SceneMgr.h"
#include "../Engine/Engine.h"
#include "Engine/Engine.h"
namespace tzw
{
	TZW_SINGLETON_IMPL(Sky)

		Sky::Sky()
	{
		m_isEnable = true;
		m_skyBoxTechnique = Material::createFromEffect("Sky");
		m_model = Model::create("Res/EngineCoreRes/Models/sphere.tzw");
		setCamera(g_GetCurrScene()->defaultCamera());
		setScale(vec3(6360000.0f, 6360000.0f, 6360000.0f));
		reCache();
		tintTex = TextureMgr::shared()->getByPath(Engine::shared()->getUserPath("CubeGame/Sky/tint.tga"));
		tintTex2 = TextureMgr::shared()->getByPath(Engine::shared()->getUserPath("CubeGame/Sky/tint2.tga"));
		sunTex = TextureMgr::shared()->getByPath(Engine::shared()->getUserPath("CubeGame/Sky/sun.tga"));
		moonTex = TextureMgr::shared()->getByPath(Engine::shared()->getUserPath("CubeGame/Sky/moon.tga"));
		cloudTex = TextureMgr::shared()->getByPath(Engine::shared()->getUserPath("CubeGame/Sky/clouds1.tga"));
		cloudTex2 = TextureMgr::shared()->getByPath(Engine::shared()->getUserPath("CubeGame/Sky/clouds2.tga"));

		m_skyBoxTechnique->setTex("tint", tintTex, 1);
		m_skyBoxTechnique->setTex("tint2", tintTex2, 2);
		m_skyBoxTechnique->setTex("sun", sunTex, 3);
		m_skyBoxTechnique->setTex("moon", moonTex, 4);
		m_skyBoxTechnique->setTex("clouds1", cloudTex, 5);
		m_skyBoxTechnique->setTex("clouds2", cloudTex2, 6);
		setWeather(1.0f);
	}

	bool Sky::isEnable() const
	{
		return m_isEnable;
	}

	void Sky::setIsEnable(bool val)
	{
		m_isEnable = val;
	}

	Material * Sky::getMaterial() const
	{
		return m_skyBoxTechnique;
	}

	void Sky::setUpTransFormation(TransformationInfo & info)
	{
		Drawable::setUpTransFormation(info);
		auto mat = m_camera->getTranslationMatrix();
		mat.data()[13] = 0.f;
		info.m_worldMatrix = mat * getLocalTransform();
		m_skyBoxTechnique->setVar("viewdir",info.m_worldMatrix.forward());
	}

	tzw::Mesh * Sky::getMesh()
	{
		return m_model->getMesh(0);
	}

	void Sky::prepare()
	{
		auto dt = Engine::shared()->deltaTime();
		static float count = 0.0f;
		count += dt * 0.003;
		m_skyBoxTechnique->setVar("time", count);
		m_skyBoxTechnique->setVar("weather", 1.0f);
	}

	float Sky::getWeather() const
	{
		return m_weather;
	}

	void Sky::setWeather(float val)
	{
		m_weather = val;
		m_skyBoxTechnique->setVar("weather", m_weather);
	}
}