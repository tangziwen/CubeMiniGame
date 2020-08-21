#pragma once
#include "vulkan/vulkan.h"
#include <unordered_set>
#include <unordered_map>
namespace tzw
{
class DeviceTextureVK;
class DeviceBufferVK;
class DeviceDescriptorSetLayoutVK
{
public:
	DeviceDescriptorSetLayoutVK(unsigned setID);
	unsigned getSetID();
	bool isHaveThisBinding(unsigned bindingID);
	unsigned getBindingFromName(std::string name);
	void addBinding(unsigned bindingId, std::string name);
	void setLayout(VkDescriptorSetLayout layout);
	VkDescriptorSetLayout getLayout();
private:
	unsigned m_setID;
	std::unordered_set<unsigned> m_bindingSet;
	std::unordered_map<std::string, unsigned> m_nameToBindingMap;
	VkDescriptorSetLayout m_layout;
};
};

