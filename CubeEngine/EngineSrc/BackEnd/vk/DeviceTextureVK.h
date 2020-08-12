#pragma once
#include "../DeviceTexture.h"
#include "vulkan/vulkan.h"
#include <string>
namespace tzw
{
enum class TextureRtFlagVK{
	NOT_TREAT_AS_RT,
	COLOR_ATTACHMENT,
	DEPTH_ATTACHEMENT
	
};
class DeviceTextureVK: public DeviceTexture
{
public:
	DeviceTextureVK(const unsigned char * buff, size_t size);
	DeviceTextureVK(std::string filepath);
	DeviceTextureVK();
	const VkImage getImage();
	const VkImageView getImageView();
	const VkSampler getSampler();
	VkImageLayout getImageLayOut();
	void initDataRaw(const unsigned char * buff, size_t w, size_t h, ImageFormat format);
	void initEmpty(size_t w, size_t h, ImageFormat format, TextureRtFlagVK rtFlag = TextureRtFlagVK::NOT_TREAT_AS_RT);
private:
	void initData(const unsigned char * buff, size_t size);
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkImageView m_textureStencilImageView;
	VkSampler m_sampler;
	VkImageLayout m_imageLayOut;
};


};

