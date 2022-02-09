#pragma once
#include "Rendering/ImageFormat.h"

namespace tzw
{
class Material;
class ShadingParams;
class DeviceDescriptor;
class DeviceShaderCollection;
class DeviceMaterial
{
public:
	virtual DeviceDescriptor * getMaterialDescriptorSet() = 0;
	virtual void updateUniform() = 0;
	virtual void initCompute(DeviceShaderCollection * shader) = 0;
	virtual void init(Material* material) = 0;
	virtual void updateUniformSingle(std::string name, void * buff, size_t size) =0;
	Material * getMat()
	{
		return m_mat;
	}
	ShadingParams * getShadingParams()
	{
		return m_shadingParams;
	}
	void setShadingParams(ShadingParams * params){ m_shadingParams = params;}
protected:
	Material * m_mat = nullptr;
	ShadingParams * m_shadingParams = nullptr;
};


};

