#include "ShadowMap.h"
#include "EngineSrc/Shader/ShaderMgr.h"
#include "EngineSrc/Lighting/DirectionalLight.h"
#include "EngineSrc/Scene/SceneMgr.h"
#define M_PI 3.141592653
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
namespace tzw
{
	TZW_SINGLETON_IMPL(ShadowMap)
	ShadowMap::ShadowMap()
	{
		m_program = ShaderMgr::shared()->getByPath("./Res/EngineCoreRes/Shaders/ShadowNaive_v.glsl", "./Res/EngineCoreRes/Shaders/ShadowNaive_f.glsl");
		m_camera = new Camera();
		int shadowMapSize[] = {512, 512, 512};
		for (int i =0; i < SHADOWMAP_CASCADE_NUM; i++)
		{
			auto shadowMap = new ShadowMapFBO();
			shadowMap->Init(shadowMapSize[i], shadowMapSize[i]);
			m_shadowMapList.push_back(shadowMap);
		}
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


	void ShadowMap::getFrustumCorners(std::vector<vec3>& corners, Matrix44 projection, float zStart, float zEnd)
	{
		corners.clear();
		auto dStart = projection * vec4(0.0f, 0.0f, -1 * zStart, 1.0f);
		dStart.z /= dStart.w;
		auto dEnd = projection * vec4(0.0f, 0.0f, -1 * zEnd, 1.0f);
		dEnd.z /= dEnd.w;
		// homogeneous corner coords
		vec4 hcorners[8];
		// far
		hcorners[0] = vec4(-1, 1, dEnd.z, 1);
		hcorners[1] = vec4(1, 1, dEnd.z, 1);
		hcorners[2] = vec4(1, -1, dEnd.z, 1);
		hcorners[3] = vec4(-1, -1, dEnd.z, 1);
		// near
		hcorners[4] = vec4(-1, 1, dStart.z, 1);
		hcorners[5] = vec4(1, 1, dStart.z, 1);
		hcorners[6] = vec4(1, -1, dStart.z, 1);
		hcorners[7] = vec4(-1, -1, dStart.z, 1);
 		auto inverseProj = projection.inverted();
		for (int i = 0; i < 8; i++) {
			hcorners[i] = inverseProj *  hcorners[i];
			hcorners[i].x /= hcorners[i].w;
			hcorners[i].y /= hcorners[i].w;
			hcorners[i].z /= hcorners[i].w;

			corners.push_back(hcorners[i].toVec3());
		}    
	}

	void ShadowMap::setupRead()
	{
		//for (int i = 0 ; i < SHADOWMAP_CASCADE_NUM ; i++) {
		//	char Name[128] = { 0 };
		//	sprintf_s(Name, sizeof(Name), "gCascadeEndClipSpace[%d]", i);
		//	m_cascadeEndClipSpace[i] = GetUniformLocation(Name);
		//}
	}

	void ShadowMap::calculateZList()
	{
		auto camera = g_GetCurrScene()->defaultCamera();
		float fov, aspect,near,far;
		camera->getPerspectInfo(&fov, & aspect, &near, &far);
		m_zlistView[0] = near;
		m_zlistView[1] = 25.0f;
		m_zlistView[2] = 75.0;
		m_zlistView[3] = far;

		auto projection = camera->projection();
		for(int i = 0; i < SHADOWMAP_CASCADE_NUM + 1; i++)
		{
			auto result = projection * vec4(0.0, 0.0, -1 * m_zlistView[i], 1.0);
			result.z /= result.w;
			m_zlistNDC[i] = result.z * 0.5 + 0.5;
		}
		return;
	}

	float ShadowMap::getCascadeEnd(int index)
	{
		return m_zlistNDC[index + 1];
	}

	Matrix44 ShadowMap::getLightProjectionMatrix(int index)
	{
		return m_projectMatList[index];
	}

	void ShadowMap::calculateProjectionMatrix()
	{
		auto camera = g_GetCurrScene()->defaultCamera();
		auto camInv = camera->getTransform();
		camera->reCache();
		auto lightView = getLightViewMatrix();
		calculateZList();
		float fov, aspect,near,far;
		camera->getPerspectInfo(&fov, & aspect, &near, &far);
		for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
		{
			AABB aabb;
			const int NUM_FRUSTUM_CORNERS = 8;
			//camera frustum corners in view space;
			std::vector<vec3> frustumCorners;
			getFrustumCorners(frustumCorners, camera->projection(), m_zlistView[i], m_zlistView[i+1]);
			camera->reCache();
			std::vector<vec3> frustumCornersL;
            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::min();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::min();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::min();
			for (unsigned j = 0 ; j < NUM_FRUSTUM_CORNERS ; j++) 
			{
				vec4 coord_in_world = camInv * vec4(frustumCorners[j], 1.0);
				vec4 coord_in_LightView =  lightView * coord_in_world;
				aabb.update(vec3(coord_in_LightView.x, coord_in_LightView.y, coord_in_LightView.z));
                minX = std::min(minX, coord_in_LightView.x);
                maxX = std::max(maxX, coord_in_LightView.x);
                minY = std::min(minY, coord_in_LightView.y);
                maxY = std::max(maxY, coord_in_LightView.y);
                minZ = std::min(minZ, coord_in_LightView.z);
                maxZ = std::max(maxZ, coord_in_LightView.z);
				frustumCornersL.push_back(coord_in_LightView.toVec3());
			}
			Matrix44 mat;
			auto min_val = aabb.min();
			auto max_val = aabb.max();
			//注意后两位和前面的差异！！！！ 
			mat.ortho(min_val.x, max_val.x, min_val.y, max_val.y, -1 * maxZ - 50,  -1 * minZ);
			m_projectMatList[i] = mat;
			aabb.reset();
		}
	}

	ShadowMapFBO * ShadowMap::getFBO(int index)
	{
		return m_shadowMapList[index];
	}

	AABB ShadowMap::getPotentialRange()
	{
		AABB aabb;
		auto camera = g_GetCurrScene()->defaultCamera();
		const int NUM_FRUSTUM_CORNERS = 8;
		//camera frustum corners in view space;
		std::vector<vec3> frustumCorners;
		float fov, aspect,near,far;
		camera->getPerspectInfo(&fov, & aspect, &near, &far);
		getFrustumCorners(frustumCorners, camera->projection(),near, far);
		auto camInv = camera->getTransform();
        for (unsigned j = 0 ; j < NUM_FRUSTUM_CORNERS ; j++) {
            vec4 coord_in_world = camInv * vec4(frustumCorners[j], 1.0);
			aabb.update(coord_in_world.toVec3());
        }
		return aabb;
	}
}
