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
	DeviceTextureVK();
	const VkImage getImage();
	const VkImageView getImageView();
	const VkSampler getSampler();
	void initDataRaw(const unsigned char * buff, size_t w, size_t h, ImageFormat format);
	void initEmpty(size_t w, size_t h, ImageFormat format);
private:
	void initData(const unsigned char * buff, size_t size);
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkSampler m_sampler;
};


};

