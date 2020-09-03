#include "DeviceDEscriptorVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceBufferVK.h"
namespace tzw
{
	DeviceDescriptorVK::DeviceDescriptorVK(VkDescriptorSet descSet, DeviceDescriptorSetLayoutVK * layout):
		m_descriptorSet(descSet), m_layout(layout)
	{
	}
	void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceTextureVK* texture)
	{
        if(!m_layout->isHaveThisBinding(binding))
        {
        
            abort();
        }
        VkDescriptorImageInfo imageInfo{};
        if(texture->getIsDepth())
        {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//texture->getImageLayOut();
        }
        else
        {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        }
        
        imageInfo.imageView = texture->getImageView();
        imageInfo.sampler = texture->getSampler();
        VkWriteDescriptorSet texWriteSet{};
        texWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        texWriteSet.dstSet = m_descriptorSet;
        texWriteSet.dstBinding = binding;
        texWriteSet.dstArrayElement = 0;
        texWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texWriteSet.descriptorCount = 1;
        texWriteSet.pImageInfo = &imageInfo;
	    vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), 1, &texWriteSet, 0, nullptr);
	}
    void DeviceDescriptorVK::updateDescriptorByBinding(int binding, std::vector<DeviceTextureVK*>& textureList)
    {
        if(!m_layout->isHaveThisBinding(binding))
        {
        
            abort();
        }
        std::vector<VkDescriptorImageInfo> imageInfoList;
        for(auto& texture : textureList)
        {
            VkDescriptorImageInfo imageInfo{};
            if(texture->getIsDepth())
            {
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            }
            else
            {
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
            }
        
            imageInfo.imageView = texture->getImageView();
            imageInfo.sampler = texture->getSampler();
            imageInfoList.push_back(imageInfo);
        }

        VkWriteDescriptorSet texWriteSet{};
        texWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        texWriteSet.dstSet = m_descriptorSet;
        texWriteSet.dstBinding = binding;
        texWriteSet.dstArrayElement = 0;
        texWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texWriteSet.descriptorCount = imageInfoList.size();
        texWriteSet.pImageInfo = imageInfoList.data();
        texWriteSet.pBufferInfo = 0;
        texWriteSet.pNext = nullptr;
	    vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), 1, &texWriteSet, 0, nullptr);
    }

    void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceBufferVK* buffer, size_t offset, size_t range)
    {
        if(!m_layout->isHaveThisBinding(binding))
        {
        
            abort();
        }
        //update descriptor
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer->getBuffer();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        VkWriteDescriptorSet writeSet{};
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.pNext = nullptr;
        writeSet.dstSet = m_descriptorSet;
        writeSet.dstBinding = binding;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeSet.descriptorCount = 1;
        writeSet.pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), 1, &writeSet, 0, nullptr);
    }
    VkDescriptorSet DeviceDescriptorVK::getDescSet()
    {
        return m_descriptorSet;
    }
}