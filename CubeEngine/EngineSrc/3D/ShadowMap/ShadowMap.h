#pragma once
#include "Engine/EngineDef.h"
#include "EngineSrc/Shader/ShaderProgram.h"
#include "Base/Camera.h"
#include "ShadowMapFBO.h"
namespace tzw
{
	class ShadowMap
	{
	public:
		TZW_SINGLETON_DECL(ShadowMap)
		ShadowMap();
		ShaderProgram * getProgram();
		Matrix44 getLightViewMatrix();
		Matrix44 getLightProjectionMatrix();
		ShadowMapFBO * getFBO();
	private:
		ShaderProgram * m_program;
		Camera * m_camera;
		ShadowMapFBO * m_shadowMap;
	};
}
