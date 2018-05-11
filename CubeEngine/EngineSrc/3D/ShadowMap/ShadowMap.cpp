#include "ShadowMap.h"
#include "EngineSrc/Shader/ShaderMgr.h"
#include "EngineSrc/Lighting/DirectionalLight.h"
#include "EngineSrc/Scene/SceneMgr.h"
#define M_PI 3.141592653
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
#include "glm/glm.hpp"
namespace tzw
{
	TZW_SINGLETON_IMPL(ShadowMap)
	ShadowMap::ShadowMap()
	{
		m_program = ShaderMgr::shared()->getByPath("./Res/EngineCoreRes/Shaders/ShadowNaive_v.glsl", "./Res/EngineCoreRes/Shaders/ShadowNaive_f.glsl");
		m_camera = new Camera();
		int shadowMapSize[] = {1024, 512, 512};
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

	Matrix44 lookAt(vec3 eye, vec3 center, vec3 up)
	{
		Matrix44 rotateInv;
		Matrix44 translateInv;
		rotateInv.setToIdentity();
		translateInv.setToIdentity();
		auto data = rotateInv.data();

		//vec3 const f((center).normalized());
		//vec3 const s(vec3::CrossProduct(f, up).normalized());
		//vec3 const u(vec3::CrossProduct(s, f));

		vec3 N = center;
		N.normalize();
		vec3 U = up;
		U = vec3::CrossProduct(U, N);
		U.normalize();
		vec3 V = vec3::CrossProduct(N, U);

		data[0] = U.x;
		data[1] = V.x;
		data[2] = N.x;
		data[3] = 0.0f;

		data[4] = U.y;
		data[5] = V.y;
		data[6] = N.y;
		data[7] = 0.0f;

		data[8] = U.z;
		data[9] = V.z;
		data[10] =N.z;
		data[11] = 0.0f;

		translateInv.translate(eye * -1.0f);
		return rotateInv * translateInv;
	}

	Matrix44 ShadowMap::getLightViewMatrix()
	{
		auto currScene = g_GetCurrScene();


		auto mat = lookAt(vec3(0, 0, 0), currScene->getDirectionLight()->dir(), vec3(0, 1, 0));
		return mat;

		
		//m_camera->setPos(0, 0, 0);
		//m_camera->lookAt(currScene->getDirectionLight()->dir() + vec3(0, 0, 0), vec3(0, 1, 0));
		//

		//m_camera->reCache();
		//return m_camera->getViewMatrix();
	}


	void ShadowMap::getFrustumCorners(std::vector<vec3>& corners, Matrix44 projection, float zStart, float zEnd)
	{
		corners.clear();
		//auto dStart = projection * vec4(0.0f, 0.0f, -1 * zStart, 1.0f);
		//dStart.z /= dStart.w;
		//auto dEnd = projection * vec4(0.0f, 0.0f, -1 * zEnd, 1.0f);
		//dEnd.z /= dEnd.w;
		//dStart.z = -1.0;
		//dEnd.z = 1.0;
		//// homogeneous corner coords
		//vec4 hcorners[8];
		//// far
		//hcorners[0] = vec4(-1, 1, dEnd.z, 1);
		//hcorners[1] = vec4(1, 1, dEnd.z, 1);
		//hcorners[2] = vec4(1, -1, dEnd.z, 1);
		//hcorners[3] = vec4(-1, -1, dEnd.z, 1);
		//// near
		//hcorners[4] = vec4(-1, 1, dStart.z, 1);
		//hcorners[5] = vec4(1, 1, dStart.z, 1);
		//hcorners[6] = vec4(1, -1, dStart.z, 1);
		//hcorners[7] = vec4(-1, -1, dStart.z, 1);
		//auto inverseProj = projection.inverted();

		////auto projMat = projection.data();
		////float near   = projMat[14] / (projMat[10] - 1.0f);
		////float far    = projMat[14] / (projMat[10] + 1.0f);
		////float bottom = near * (projMat[9] - 1.0f) / projMat[5];
		////float top    = near * (projMat[9] + 1.0f) / projMat[5];
		////float left   = near * (projMat[8] - 1.0f) / projMat[0];
		////float right  = near * (projMat[8] + 1.0f) / projMat[0];
		////
		//for (int i = 0; i < 8; i++) {
		//	hcorners[i] = inverseProj *  hcorners[i];
		//	hcorners[i].x /= hcorners[i].w;
		//	hcorners[i].y /= hcorners[i].w;
		//	hcorners[i].z /= hcorners[i].w;

		//	corners.push_back(hcorners[i].toVec3());
		//}

			auto camera = g_GetCurrScene()->defaultCamera();
			float fov, aspect,near,far;
			camera->getPerspectInfo(&fov, & aspect, &near, &far);
	        float ar = 1024.0f / 1024.0f;
	        float tanHalfHFOV = tanf(ToRadian(fov / 2.0f));
	        float tanHalfVFOV = tanf(ToRadian((fov * ar) / 2.0f));
			float xn = zStart * tanHalfHFOV;
            float xf = zEnd * tanHalfHFOV;
            float yn = zStart * tanHalfVFOV;
            float yf = zEnd * tanHalfVFOV;

			// near face
			corners.push_back(vec3(xn,   yn, zStart));
			corners.push_back(vec3(-xn,  yn, zStart));
			corners.push_back(vec3(xn,  -yn, zStart));
			corners.push_back(vec3(-xn, -yn, zStart));
                
			// far face
			corners.push_back(vec3(xf,   yf, zEnd));
			corners.push_back(vec3(-xf,  yf, zEnd));
			corners.push_back(vec3(xf,  -yf, zEnd));
			corners.push_back(vec3(-xf, -yf, zEnd));     
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
			auto result = projection * vec4(0.0, 0.0, m_zlistView[i], 1.0);
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

		////just for test.
		//camera->setPos(8.0, 21.0, -23.0);
		//camera->lookAt(vec3(-0.07f, -0.44f, 0.9f) + vec3(8.0, 21.0, -23.0), vec3(0.0, 1.0f, 0.0f));

		auto camInv = camera->getViewMatrix().inverted();
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
			
			

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::min();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::min();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::min();
			std::vector<vec3> frustumCornersL;
			for (unsigned j = 0 ; j < NUM_FRUSTUM_CORNERS ; j++) 
			{
				vec4 coord_in_world = camInv * vec4(frustumCorners[j], 1.0);
				//frustumCornersW.push_back(coord_in_world.toVec3());
				
				vec4 coord_in_LightView =  lightView * coord_in_world;
				aabb.update(vec3(coord_in_LightView.x, coord_in_LightView.y, coord_in_LightView.z));
				frustumCornersL.push_back(coord_in_LightView.toVec3());
			    minX = std::min(minX, frustumCornersL[j].x);
	            maxX = std::max(maxX, frustumCornersL[j].x);
	            minY = std::min(minY, frustumCornersL[j].y);
	            maxY = std::max(maxY, frustumCornersL[j].y);
	            minZ = std::min(minZ, frustumCornersL[j].z);
	            maxZ = std::max(maxZ, frustumCornersL[j].z);
				
			}
			Matrix44 mat;
			auto min_val = aabb.min();
			auto max_val = aabb.max();
			//mat.ortho(-50, 50, -50, 50, -200, 200);

			mat.ortho(minX, maxX, minY, maxY, minZ, maxZ);
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
