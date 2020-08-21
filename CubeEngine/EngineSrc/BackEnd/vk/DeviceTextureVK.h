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
	bool getIsDepth();
	void initDataRaw(const unsigned char * buff, size_t w, size_t h, ImageFormat format);
	void initEmpty(size_t w, size_t h, ImageFormat format, TextureRtFlagVK rtFlag = TextureRtFlagVK::NOT_TREAT_AS_RT);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
private:
	bool m_isDepth;
	std::string m_filePath;
	void initData(const unsigned char * buff, size_t size);
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkImageView m_textureStencilImageView;
	VkSampler m_sampler;
	VkImageLayout m_imageLayOut;
};


};

