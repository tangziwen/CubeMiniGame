#include "ShadowMap.h"
#include "EngineSrc/Shader/ShaderMgr.h"
#include "EngineSrc/Lighting/DirectionalLight.h"
#include "EngineSrc/Scene/SceneMgr.h"
#define M_PI 3.141592653
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
#include <algorithm>
namespace tzw
{
	ShadowMap::ShadowMap()
	{
		//m_program = ShaderMgr::shared()->getByPath(0, "Shaders/ShadowNaive_v.glsl", "Shaders/ShadowNaive_f.glsl");
		//m_InstancedProgram = ShaderMgr::shared()->getByPath(static_cast<uint32_t>(ShaderOption::EnableInstanced), "Shaders/ShadowNaive_v.glsl", "Shaders/ShadowNaive_f.glsl");
		m_camera = new Camera();
		m_shadowMapSize = 2048;

		if(!EngineDef::isUseVulkan)
		{
			for (int i =0; i < SHADOWMAP_CASCADE_NUM; i++)
			{
				auto shadowMap = new ShadowMapFBO();
				shadowMap->Init(m_shadowMapSize, m_shadowMapSize);
				m_shadowMapList.push_back(shadowMap);
			}
		}
	}

	ShaderProgram * ShadowMap::getProgram()
	{
		return m_program;
	}

	ShaderProgram* ShadowMap::getInstancedProgram()
	{
		return m_InstancedProgram;
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
		m_zlistView[1] = 5.0f;
		m_zlistView[2] = 35.0f;
		m_zlistView[3] = far;

		auto projection = camera->projection();
		for(int i = 0; i < SHADOWMAP_CASCADE_NUM + 1; i++)
		{
			auto result = projection * vec4(0.0, 0.0, -1 * m_zlistView[i], 1.0);
			result.z /= result.w;
			if(EngineDef::isUseVulkan)
			{
				m_zlistNDC[i] = result.z;
			}else{
				m_zlistNDC[i] = result.z * 0.5f + 0.5f;
			
			}

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

	float snap(float src, float tileSize)
	{
		return floor(src / tileSize) * tileSize;
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
			getFrustumCorners(frustumCorners, camera->projection(), m_zlistView[0], m_zlistView[i+1]);
			camera->reCache();
			//AABB worldAABB;
			for (unsigned j = 0 ; j < NUM_FRUSTUM_CORNERS ; j++) 
			{
				vec4 coord_in_world = camInv * vec4(frustumCorners[j], 1.0);
				aabb.update(coord_in_world.toVec3());
				//vec4 coord_in_LightView =  lightView * coord_in_world;
				//aabb.update(vec3(coord_in_LightView.x, coord_in_LightView.y, coord_in_LightView.z));
			}

			aabb.transForm(lightView);
			Matrix44 mat;
			auto min_val = aabb.min();
			auto max_val = aabb.max();



			//fit to scene
			vec3 vDiagonal = frustumCorners[6] - frustumCorners[0];
			vDiagonal = vec3(vDiagonal.length(), vDiagonal.length(), vDiagonal.length());
			float fCascadeBound = vDiagonal.x;
			vec3 halfVec = vec3(0.5, 0.5, 0.5);
            vec3 vBoarderOffset = ( vDiagonal - ( max_val - min_val ) ) * halfVec;
			vBoarderOffset.z = 0.0;
            // Add the offsets to the projection.
            max_val += vBoarderOffset;
            min_val -= vBoarderOffset;			

			vec2 vWorldUnitsPerTexel;
            // The world units per texel are used to snap the shadow the orthographic projection
            // to texel sized increments.  This keeps the edges of the shadows from shimmering.
            float fWorldUnitsPerTexel = fCascadeBound / (float)m_shadowMapSize;
            vWorldUnitsPerTexel = vec2( fWorldUnitsPerTexel, fWorldUnitsPerTexel);


			//snap to texel
			min_val.x = snap(min_val.x, vWorldUnitsPerTexel.x);
			min_val.y = snap(min_val.y, vWorldUnitsPerTexel.y);

			max_val.x = snap(max_val.x, vWorldUnitsPerTexel.x);
			max_val.y = snap(max_val.y, vWorldUnitsPerTexel.y);


			//look at the last 2 parameters! 
			mat.ortho(min_val.x, max_val.x, min_val.y, max_val.y, -1 * max_val.z - 50,  -1 * min_val.z);
			m_projectMatList[i] = mat;
			aabb.reset();
		}
	}

	ShadowMapFBO * ShadowMap::getFBO(int index)
	{
		return m_shadowMapList[index];
	}

	AABB ShadowMap::getPotentialRange(int index)
	{
		AABB aabb;
		auto camera = g_GetCurrScene()->defaultCamera();
		const int NUM_FRUSTUM_CORNERS = 8;
		//camera frustum corners in view space;
		std::vector<vec3> frustumCorners;
		float fov, aspect,near,far;
		camera->getPerspectInfo(&fov, & aspect, &near, &far);

		near = m_zlistView[index];
		far = m_zlistView[index + 1];
		getFrustumCorners(frustumCorners, camera->projection(),near, far);
		auto camInv = camera->getTransform();
		auto lightView = getLightViewMatrix();
        for (unsigned j = 0 ; j < NUM_FRUSTUM_CORNERS ; j++) {
            vec4 coord_in_world = camInv * vec4(frustumCorners[j], 1.0);
			vec4 coord_in_LightView =  lightView * coord_in_world;
			aabb.update(coord_in_world.toVec3());
        }
		//in the light space, we modify the max_z & min_z
		auto theMax = aabb.max();
		auto theMin = aabb.min();

		vec3 wDiagonal = frustumCorners[0] - frustumCorners[6];
		float length = wDiagonal.length() + 20.0f;//some extra distance
		wDiagonal = vec3(length, length, length);
		float fCascadeBound = wDiagonal.x;
		vec3 halfVec = vec3(0.5, 0.5, 0.5);
        vec3 vBoarderOffset = ( wDiagonal - ( theMax - theMin ) ) * halfVec;
		vBoarderOffset.z = 0.0;
        // Add the offsets to the projection.
        theMax += vBoarderOffset;
        theMin -= vBoarderOffset;

		aabb.update(theMax);
		aabb.update(theMin);

		return aabb;
	}
}
