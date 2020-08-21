#include "DeviceDEscriptorVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceBufferVK.h"
namespace tzw
{
	DeviceDescriptorVK::DeviceDescriptorVK(VkDescriptorSet descSet):
		m_descriptorSet(descSet)
	{
	}
	void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceTextureVK* texture)
	{
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
    void DeviceDescriptorVK::updateDescriptorByBinding(int binding, DeviceBufferVK* buffer, size_t offset, size_t range)
    {
        //update descriptor
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer->getBuffer();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        VkWriteDescriptorSet writeSet{};
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.pNext = nullptr;
        writeSet.dstSet = m_descriptorSet;
        writeSet.dstBinding = 0;
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