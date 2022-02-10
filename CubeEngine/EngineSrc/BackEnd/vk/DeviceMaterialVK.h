#pragma once
#include "../DeviceMaterial.h"
#include "vulkan/vulkan.h"
#include <string>
#include "DeviceDescriptorVK.h"
namespace tzw
{
class DeviceShaderCollectionVK;
class DeviceMaterialVK: public DeviceMaterial
{
public:
	DeviceMaterialVK();
	virtual void init(Material* material) override;
	virtual void initCompute(DeviceShaderCollection * shader) override;
	virtual DeviceDescriptor * getMaterialDescriptorSet() override;
	virtual void updateUniform() override;
	void updateMaterialDescriptorSet();
	virtual void updateUniformSingle(std::string name, void * buff, size_t size) override;
private:
	void createMaterialDescriptorPool();
	void createMaterialUniformBuffer();
	void createMaterialDescriptorSet();
	DeviceDescriptorVK * m_materialDescripotrSet;
	VkDescriptorPool m_materialDescriptorPool;
	VkBuffer m_matUniformBuffer;
	VkDeviceMemory m_matUniformBufferMemory;
	DeviceShaderCollectionVK * m_shader;
};


};

