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
	DeviceTextureVK(VkImage image, VkImageView view);
	const VkImage getImage();
	const VkImageView getImageView();
	const VkSampler getSampler();
	VkImageLayout getImageLayOut();
	bool getIsDepth();
	void initDataRaw(const unsigned char * buff, size_t w, size_t h, ImageFormat format);
	void initEmpty(size_t w, size_t h, ImageFormat format, TextureRoleEnum rtFlag = TextureRoleEnum::AS_COLOR, TextureUsageEnum texUsage = TextureUsageEnum::SAMPLE_ONLY);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
private:
	void initData(const unsigned char * buff, size_t size);
	bool m_isDepth = false;
	std::string m_filePath;
	VkImage m_textureImage = {};
	VkDeviceMemory m_textureImageMemory = {};
	VkImageView m_textureImageView = {};
	VkImageView m_textureStencilImageView = {};
	VkSampler m_sampler = {};
	VkImageLayout m_imageLayOut = {};
};


};

