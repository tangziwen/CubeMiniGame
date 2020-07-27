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

void DeviceTextureVK::initData(const unsigned char* buff, size_t size)
{
    auto backEnd = VKRenderBackEnd::shared();
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load_from_memory(buff, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) 
    {
        abort();
        //throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VKRenderBackEnd::shared()->createVKBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(backEnd->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(backEnd->getDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    backEnd->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

    backEnd->transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        backEnd->copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    backEnd->transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(backEnd->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(backEnd->getDevice(), stagingBufferMemory, nullptr);

    m_textureImageView = backEnd->createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB);
}



}
