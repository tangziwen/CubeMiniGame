#pragma once
#include <string>
#include "Rendering/ImageFormat.h"
#include "Technique/MaterialTechnique.h"

namespace tzw
{
class MaterialInstance;
class ShadingParams;
class DeviceDescriptor;
class DeviceShaderCollection;
class DeviceMaterial
{
public:
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
protected:
	MaterialInstance * m_mat = nullptr;
	ShadingParams * m_shadingParams = nullptr;
};


};

