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
	void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceTexture* texture)
	{
        DeviceTextureVK * vkTex =static_cast<DeviceTextureVK *>(texture);
        if(!m_layout->isHaveThisBinding(binding))
        {
        
            abort();
        }
        VkDescriptorImageInfo imageInfo{};
        if(vkTex->getIsDepth())
        {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//texture->getImageLayOut();
        }
        else
        {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        }
        
        imageInfo.imageView = vkTex->getImageView();
        imageInfo.sampler = vkTex->getSampler();
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
    void DeviceDescriptorVK::updateDescriptorByBinding(int binding, std::vector<DeviceTexture*>& textureList)
    {
        if(!m_layout->isHaveThisBinding(binding))
        {
        
            abort();
        }
        std::vector<VkDescriptorImageInfo> imageInfoList;
        for(auto& texture : textureList)
        {
            DeviceTextureVK * vkTex =static_cast<DeviceTextureVK *>(texture);
            VkDescriptorImageInfo imageInfo{};
            if(vkTex->getIsDepth())
            {
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            }
            else
            {
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
            }
        
            imageInfo.imageView = vkTex->getImageView();
            imageInfo.sampler = vkTex->getSampler();
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

    void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceBuffer* buffer, size_t offset, size_t range)
    {
        if(!m_layout->isHaveThisBinding(binding))
        {
        
            abort();
        }
        DeviceBufferVK * vkBuf = static_cast<DeviceBufferVK *>(buffer);
        //update descriptor
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = vkBuf->getBuffer();
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
    void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceItemBuffer * itemBuff)
    {
        updateDescriptorByBinding(binding, itemBuff->m_pool->getBuffer(), itemBuff->m_offset, itemBuff->m_size);
    }
    VkDescriptorSet DeviceDescriptorVK::getDescSet()
    {
        return m_descriptorSet;
    }
}