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
	~DeviceMaterialVK() override;
	virtual void init(MaterialInstance* material, MaterialTechniqueType techniqueType = MaterialTechniqueType::Default) override;
	virtual void initCompute(DeviceShaderCollection * shader) override;
	virtual DeviceDescriptor * getMaterialDescriptorSet() override;
	virtual void updateUniform() override;
	virtual void updateMaterialDescriptorSet() override;
	virtual void updateUniformSingle(std::string name, void * buff, size_t size) override;
private:
	void createMaterialDescriptorPool();
	void createMaterialUniformBuffer();
	void createMaterialDescriptorSet();
	DeviceDescriptorVK * m_materialDescripotrSet = nullptr;
	VkDescriptorPool m_materialDescriptorPool = VK_NULL_HANDLE;
	VkBuffer m_matUniformBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_matUniformBufferMemory = VK_NULL_HANDLE;
	DeviceShaderCollectionVK * m_shader = nullptr;
};


};

