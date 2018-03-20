#include "ShadowMap.h"
#include "EngineSrc/Shader/ShaderMgr.h"
#include "EngineSrc/Lighting/DirectionalLight.h"
#include "EngineSrc/Scene/SceneMgr.h"
namespace tzw
{
	TZW_SINGLETON_IMPL(ShadowMap)
	ShadowMap::ShadowMap()
	{
		m_program = ShaderMgr::shared()->getByPath("./Res/EngineCoreRes/Shaders/ShadowNaive_v.glsl", "./Res/EngineCoreRes/Shaders/ShadowNaive_f.glsl");
		m_camera = new Camera();
		m_shadowMap = new ShadowMapFBO();
		m_shadowMap->Init(1024, 768);
	}

	ShaderProgram * ShadowMap::getProgram()
	{
		return m_program;
	}
	Matrix44 ShadowMap::getLightViewMatrix()
	{
		auto currScene = g_GetCurrScene();
		m_camera->setPos(0, 0, 0);
		m_camera->lookAt(currScene->getDirectionLight()->dir(), vec3(0, 1, 0));
		m_camera->reCache();
		return m_camera->getViewMatrix();
	}
	Matrix44 ShadowMap::getLightProjectionMatrix()
	{
		Matrix44 mat;
		mat.ortho(-50, 50, -50, 50, -50, 50);
		return mat;
	}

	ShadowMapFBO * ShadowMap::getFBO()
	{
		return m_shadowMap;
	}
}