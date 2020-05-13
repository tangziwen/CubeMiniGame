#pragma once
#include "Engine/EngineDef.h"
#include "EngineSrc/Shader/ShaderProgram.h"
#include "Base/Camera.h"
#include "ShadowMapFBO.h"

#define SHADOWMAP_CASCADE_NUM 3
namespace tzw
{
	class ShadowMap : public Singleton<ShadowMap>
	{
	public:
		ShadowMap();
		ShaderProgram * getProgram();
		ShaderProgram * getInstancedProgram();
		Matrix44 getLightViewMatrix();
		Matrix44 getLightProjectionMatrix(int index);
		void calculateProjectionMatrix();
		ShadowMapFBO * getFBO(int index);
		AABB getPotentialRange(int index);
		void getFrustumCorners(std::vector<vec3>& corners, Matrix44 projection, float zStart, float zEnd);
		void setupRead();
		void calculateZList();
		float getCascadeEnd(int index);
	private:
		ShaderProgram * m_program;
		ShaderProgram * m_InstancedProgram;
		Camera * m_camera;
		Matrix44 m_projectMatList[SHADOWMAP_CASCADE_NUM];
		std::vector<ShadowMapFBO *> m_shadowMapList;
		float m_zlistView[SHADOWMAP_CASCADE_NUM + 1]{};
		float m_zlistNDC[SHADOWMAP_CASCADE_NUM + 1]{};
	};
}
