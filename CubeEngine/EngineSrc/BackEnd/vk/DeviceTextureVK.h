#pragma once
#include "../DeviceTexture.h"
#include "vulkan/vulkan.h"
#include <string>
namespace tzw
{
class DeviceTextureVK: public DeviceTexture
{
public:
	DeviceTextureVK(const unsigned char * buff, size_t size);
	DeviceTextureVK(std::string filepath);
	const VkImage& getImage();
	const VkImageView & getImageView();
private:
	void initData(const unsigned char * buff, size_t size);
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
};


};

