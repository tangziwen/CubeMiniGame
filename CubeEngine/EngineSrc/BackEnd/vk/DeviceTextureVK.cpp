#include <cstdint>
#include "gli/gli.hpp"
#include "DeviceTextureVK.h"
#include "SOIL2/stb_image.h"
#include "../VkRenderBackEnd.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw
{
DeviceTextureVK::DeviceTextureVK(const unsigned char* buff, size_t size)
{
    initData(buff, size);
}

DeviceTextureVK::DeviceTextureVK(std::string filepath)
{
    m_filePath = filepath;
	if(filepath.find("terrain_atlas_roughness") != std::string::npos)
	{
		tlog("shit");
	}
    auto data =Tfile::shared()->getData(filepath,false);
    initData(data.getBytes(), data.getSize());
}

DeviceTextureVK::DeviceTextureVK()
{

}

DeviceTextureVK::DeviceTextureVK(VkImage image, VkImageView view):m_isDepth(false)
{
    m_textureImage = image;
    m_textureImageView = view;
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

bool DeviceTextureVK::getIsDepth()
{
    return m_isDepth;
}


void DeviceTextureVK::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) 
{
        auto backEnd = VKRenderBackEnd::shared();
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(backEnd->GetPhysDevice(), imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            //throw std::runtime_error("texture image format does not support linear blitting!");
            abort();
        }

        VkCommandBuffer commandBuffer = backEnd->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        backEnd->endSingleTimeCommands(commandBuffer);
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
VkImageAspectFlags DeviceTextureVK::getImageAspectFlag()
{
    VkImageAspectFlags flag;
    switch(m_textureRole)
    {
    case TextureRoleEnum::AS_COLOR:
        flag = VK_IMAGE_ASPECT_COLOR_BIT;
        break;
    case TextureRoleEnum::AS_DEPTH:
        flag = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        break;
    }
    return flag;
}

void DeviceTextureVK::initEmpty(size_t texWidth, size_t texHeight, ImageFormat format, TextureRoleEnum texRole, TextureUsageEnum texUsage, int mipLevels)
{

    auto backEnd = VKRenderBackEnd::shared();
    VkDeviceSize imageSize = texWidth * texHeight * ImageFormatGetSize(format);
    
	
	m_metaInfo.width = texWidth;
	m_metaInfo.height = texHeight;
    m_textureRole = texRole;
	m_textureUsage = texUsage;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkFormat vkformat = VKRenderBackEnd::shared()->getFormat(format);
    VKRenderBackEnd::shared()->createVKBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    VkImageUsageFlags usageFlag = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    switch(m_textureUsage)
    {
    case TextureUsageEnum::SAMPLE_ONLY:
        usageFlag |= 0;
    	if(m_textureRole == TextureRoleEnum::AS_DEPTH)
    	{
    		m_isDepth = true;
    	}
        break;
    case TextureUsageEnum::SAMPLE_AND_ATTACHMENT:
        if(m_textureRole == TextureRoleEnum::AS_COLOR)
        {
	        usageFlag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
        }
        else if(m_textureRole == TextureRoleEnum::AS_DEPTH)
        {
	        usageFlag |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
        	m_isDepth = true;
        }
        break;
    }
    VkImageAspectFlags flag = getImageAspectFlag();
    if(mipLevels < 0)//auto detect mip levels
    {
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    }
    backEnd->createImage(texWidth, texHeight, vkformat, VK_IMAGE_TILING_OPTIMAL, usageFlag | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory, mipLevels);

	//set layout
    if(m_textureUsage ==TextureUsageEnum::SAMPLE_AND_ATTACHMENT)
    {
    	if(m_textureRole == TextureRoleEnum::AS_DEPTH)
    	{
	        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
	        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, flag);
	        m_imageLayOut = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    	}
        else if (m_textureRole == TextureRoleEnum::AS_COLOR)
        {
	        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
	        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, flag);
	        m_imageLayOut = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

    }
    else if(m_textureUsage ==TextureUsageEnum::SAMPLE_ONLY)
    {
    	if(m_textureRole == TextureRoleEnum::AS_COLOR)
    	{
		    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
		        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), flag);
		    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, flag);
		        m_imageLayOut = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    	}
		else if(m_textureRole == TextureRoleEnum::AS_DEPTH)
        {
		    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, flag);
		        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), flag);
		    backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, flag);
		        m_imageLayOut = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        }
    }
    

    vkDestroyBuffer(backEnd->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(backEnd->getDevice(), stagingBufferMemory, nullptr);


    if(m_textureRole ==TextureRoleEnum::AS_DEPTH)
    {
        m_textureImageView = backEnd->createImageView(m_textureImage, vkformat, VK_IMAGE_ASPECT_DEPTH_BIT);
        m_textureStencilImageView = backEnd->createImageView(m_textureImage, vkformat, VK_IMAGE_ASPECT_STENCIL_BIT);
    }
    else
    {
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
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* pixels = stbi_load_from_memory(buff, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    gli::texture t =  gli::load((const char *)buff, size);
    int theBaselevel = t.base_level();
    printf("the level %d %d\n",t.base_level(),t.max_level());
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    size_t baseMip = 0;

    size_t mipLevels = 1;
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
    if(t.empty())
    {
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        VKRenderBackEnd::shared()->createVKBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(backEnd->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(backEnd->getDevice(), stagingBufferMemory);

        stbi_image_free(pixels);

        backEnd->createImage(texWidth, texHeight, vkformat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory, mipLevels);

        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels);
        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        generateMipmaps(m_textureImage, vkformat, texWidth, texHeight, mipLevels);
    }else
    {
        vkformat = static_cast<VkFormat>(t.format());
        backEnd->createImage(texWidth, texHeight, vkformat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT |  VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory, t.levels());
        VKRenderBackEnd::shared()->createVKBuffer(static_cast<size_t>(t.size(0)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, t.base_level(), t.levels());
        for(int i = t.base_level(); i <= t.max_level(); i++)
        {
            auto size = t.size(i);
            void* data;
            vkMapMemory(backEnd->getDevice(), stagingBufferMemory, 0, static_cast<size_t>(t.size(i)), 0, &data);
                memcpy(data, t.data(0, 0, i), static_cast<size_t>(t.size(i)));
            vkUnmapMemory(backEnd->getDevice(), stagingBufferMemory);
            backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(t.extent(i).x), static_cast<uint32_t>(t.extent(i).y), VK_IMAGE_ASPECT_COLOR_BIT, i);
            
        }
        backEnd->transitionImageLayout(m_textureImage, vkformat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,t.base_level(), t.levels());

        baseMip = t.base_level();
        mipLevels = t.levels();
    }

    vkDestroyBuffer(backEnd->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(backEnd->getDevice(), stagingBufferMemory, nullptr);
    m_imageLayOut = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    m_textureImageView = backEnd->createImageView(m_textureImage, vkformat, VK_IMAGE_ASPECT_COLOR_BIT,baseMip, mipLevels);



    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.minLod = baseMip;
    samplerInfo.maxLod = baseMip + mipLevels - 1;
    samplerInfo.mipLodBias = 0;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(backEnd->getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) 
    {
        abort();
    }
}



}
