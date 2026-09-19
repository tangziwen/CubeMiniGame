#pragma once
#include <string>
#include "Math/vec2.h"
#include "Rendering/ImageFormat.h"
#include "Technique/MaterialTechnique.h"

namespace tzw
{
class Camera;
class MaterialInstance;
class ShadingParams;
class DeviceDescriptor;
class DeviceShaderCollection;
class DeviceMaterial
{
public:
	virtual ~DeviceMaterial() = default;
	virtual DeviceDescriptor * getMaterialDescriptorSet() = 0;
	virtual void updateUniform() = 0;
	virtual void initCompute(DeviceShaderCollection * shader) = 0;
	virtual void init(MaterialInstance* material, MaterialTechniqueType techniqueType = MaterialTechniqueType::Default) = 0;
	virtual void updateUniformSingle(std::string name, void * buff, size_t size) =0;
	virtual void updateMaterialDescriptorSet() = 0;
	MaterialInstance * getMat()
	{
		return m_mat;
	}
	ShadingParams * getShadingParams()
	{
		return m_shadingParams;
	}
	void setShadingParams(ShadingParams * params){ m_shadingParams = params;}
void setView(Camera* camera, vec2 size) { m_viewCamera = camera; m_viewSize = size; }
protected:
	Camera* m_viewCamera = nullptr;
	vec2 m_viewSize = vec2(0, 0);
	MaterialInstance * m_mat = nullptr;
	ShadingParams * m_shadingParams = nullptr;
};


};

