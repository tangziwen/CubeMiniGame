#include "DeviceTextureVK.h"
#include "SOIL2/stb_image.h"
#include "../VkRenderBackEnd.h"
#include "Utility/file/Tfile.h"
namespace tzw
{
DeviceTextureVK::DeviceTextureVK(const unsigned char* buff, size_t size)
{
    initData(buff, size);
}

DeviceTextureVK::DeviceTextureVK(std::string filepath)
{
    auto data =Tfile::shared()->getData(filepath,false);
    initData(data.getBytes(), data.getSize());
}

DeviceTextureVK::DeviceTextureVK()
{
}

const VkImage DeviceTextureVK::getImage()
{
    return m_textureImage;
}

const VkImageView DeviceTextureVK::getImageView()
{
    return m_textureImageView;
}

const VkSampler DeviceTextureVK::getSampler()
{
    return m_sampler;
}

VkImageLayout DeviceTextureVK::getImageLayOut()
{
    return m_imageLayOut;
}

void DeviceTextureVK::initDataRaw(const unsigned char * buff, size_t texWidth, size_t texHeight, ImageFormat format)
{
    auto backEnd = VKRenderBackEnd::shared();
    const unsigned char * pixels = buff;
    VkDeviceSize imageSize = texWidth * texHeight * ImageFormatGetSize(format);

	m_metaInfo.width = texWidth;
	m_metaInfo.height = texHeight;
    if (!pixels) 
    {
        abort();
        //throw std::runtime_error("failed to load texture image!");
    }
    VkFormat vkformat = VKRenderBackEnd::shared()->getFormat(format);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VKRenderBackEnd::shared()->createVKBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(backEnd->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(backEnd->getDevice(), stagingBufferMemory);

    backEnd->createImage(texWidth, texHeight, vkformat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    m_imageLayOut = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkDestroyBuffer(backEnd->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(backEnd->getDevice(), stagingBufferMemory, nullptr);

    m_textureImageView = backEnd->createImageView(m_textureImage, vkformat);



     VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(backEnd->getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        abort();
    }
}

void DeviceTextureVK::initEmpty(size_t texWidth, size_t texHeight, ImageFormat format, TextureRtFlagVK rtFlag)
{

    auto backEnd = VKRenderBackEnd::shared();
    VkDeviceSize imageSize = texWidth * texHeight * ImageFormatGetSize(format);

	m_metaInfo.width = texWidth;
	m_metaInfo.height = texHeight;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkFormat vkformat = VKRenderBackEnd::shared()->getFormat(format);
    VKRenderBackEnd::shared()->createVKBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    VkImageUsageFlags usageFlag = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    switch(rtFlag)
    {
    case TextureRtFlagVK::NOT_TREAT_AS_RT:
        usageFlag |= 0;
        break;
    case TextureRtFlagVK::COLOR_ATTACHMENT:
        usageFlag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        break;
    case TextureRtFlagVK::DEPTH_ATTACHEMENT:
        usageFlag |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        break;
    }
    VkImageAspectFlags flag = 0;

    switch(rtFlag)
    {
    case TextureRtFlagVK::NOT_TREAT_AS_RT:
        flag = VK_IMAGE_ASPECT_COLOR_BIT;
        break;
    case TextureRtFlagVK::COLOR_ATTACHMENT:
        flag = VK_IMAGE_ASPECT_COLOR_BIT;
        break;
    case TextureRtFlagVK::DEPTH_ATTACHEMENT:
        flag = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        break;
    }
    backEnd->createImage(texWidth, texHeight, vkformat, VK_IMAGE_TILING_OPTIMAL, usageFlag, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

    if(rtFlag ==TextureRtFlagVK::DEPTH_ATTACHEMENT)
    {
        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, flag);
        m_imageLayOut = VK_IMAGE_LAYOUT_GENERAL;
    }
    else if(rtFlag ==TextureRtFlagVK::COLOR_ATTACHMENT)
    {
        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, flag);
        m_imageLayOut = VK_IMAGE_LAYOUT_GENERAL;
    }
    else{
    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), flag);
    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, flag);
        m_imageLayOut = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    

    vkDestroyBuffer(backEnd->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(backEnd->getDevice(), stagingBufferMemory, nullptr);


    if(rtFlag ==TextureRtFlagVK::DEPTH_ATTACHEMENT)
    {
        m_textureImageView = backEnd->createImageView(m_textureImage, vkformat, VK_IMAGE_ASPECT_DEPTH_BIT);
        m_textureStencilImageView = backEnd->createImageView(m_textureImage, vkformat, VK_IMAGE_ASPECT_STENCIL_BIT);
    }
    else{
        m_textureImageView = backEnd->createImageView(m_textureImage, vkformat, flag);
    }
    



    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(backEnd->getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        abort();
    }
}

void DeviceTextureVK::initData(const unsigned char* buff, size_t size)
{
    auto backEnd = VKRenderBackEnd::shared();
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load_from_memory(buff, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

	m_metaInfo.width = texWidth;
	m_metaInfo.height = texHeight;
    if (!pixels) 
    {
        abort();
        //throw std::runtime_error("failed to load texture image!");
    }
    VkFormat vkformat = VKRenderBackEnd::shared()->getFormat(ImageFormat::R8G8B8A8);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VKRenderBackEnd::shared()->createVKBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(backEnd->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(backEnd->getDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    backEnd->createImage(texWidth, texHeight, vkformat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    m_imageLayOut = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkDestroyBuffer(backEnd->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(backEnd->getDevice(), stagingBufferMemory, nullptr);

    m_textureImageView = backEnd->createImageView(m_textureImage, vkformat);



     VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(backEnd->getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        abort();
    }
}



}
