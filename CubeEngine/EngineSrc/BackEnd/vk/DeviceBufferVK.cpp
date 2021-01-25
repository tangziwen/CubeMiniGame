#include "DeviceBufferVK.h"
#include "../VkRenderBackEnd.h"

namespace tzw
{
    DeviceBufferVK::DeviceBufferVK()
    {
        m_buffer = VK_NULL_HANDLE;
        m_memory = VK_NULL_HANDLE;
        m_alignment = 0;
        m_bufferSize = 0;
        m_isUsePool = false;
        m_offset = 0;
    }
    void DeviceBufferVK::allocate(void* data, size_t ammount)
	{
        auto device = VKRenderBackEnd::shared()->getDevice();
        allocateEmpty(ammount);
        void* mapData;
        if(!m_isUsePool)
        {
            vkMapMemory(device, m_memory, m_offset, ammount, 0, &mapData);
                memcpy(mapData, data, (size_t) ammount);
            vkUnmapMemory(device, m_memory);
        }
        else
        {
            auto memoryPool = VKRenderBackEnd::shared()->getMemoryPool();
            memoryPool->map(m_bufferInfo, &mapData);
            memcpy(mapData, data, (size_t) ammount);
            memoryPool->unmap(m_bufferInfo);
        }

	}

    void DeviceBufferVK::allocateEmpty(size_t ammount)
    {
        if(!m_isUsePool)
        {
            allocateEmptySingleImp(ammount);
        }
        else
        {
            allocateEmptyPoolImp(ammount);
        }   
    }
    void DeviceBufferVK::allocateEmptySingleImp(size_t ammount)
    {
        VkResult res = VK_SUCCESS;
        auto device = VKRenderBackEnd::shared()->getDevice();
        if(m_buffer != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE && (ammount <= m_bufferSize))
        {
            return;
        }
        if (m_buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(device, m_buffer, nullptr);
        if (m_memory != VK_NULL_HANDLE)
            vkFreeMemory(device, m_memory, nullptr);
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        if(m_alignment > 0)
        {
            bufferInfo.size = ((ammount - 1) / m_alignment + 1) * m_alignment;
        }
        else
        {
            bufferInfo.size = ammount;
        }
        m_bufferSize = bufferInfo.size;
        if(m_type == DeviceBufferType::Vertex){
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        
        }
        else if(m_type == DeviceBufferType::Index){
            bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        else if(m_type == DeviceBufferType::Uniform){
            bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        res = vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer);
        if(res){
        
        abort();
        }
        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.memoryTypeIndex = 0;

        alloc_info.allocationSize = memRequirements.size;

        bool pass = VKRenderBackEnd::shared()->memory_type_from_properties(memRequirements.memoryTypeBits,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &alloc_info.memoryTypeIndex);
 
        res = vkAllocateMemory(device, &alloc_info, NULL,
                               &(m_memory));
        //printf("allocate Memory %zd\n", m_bufferSize);
        if(res){
        
        abort();
        }
        res = vkBindBufferMemory(device, m_buffer, m_memory, 0);
        if(res){
        
        abort();
        }
    }
    void DeviceBufferVK::allocateEmptyPoolImp(size_t ammount)
    {
        VkResult res = VK_SUCCESS;
        auto device = VKRenderBackEnd::shared()->getDevice();
        if(m_buffer != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE && (ammount <= m_bufferSize))
        {
            return;
        }
        auto memoryPool = VKRenderBackEnd::shared()->getMemoryPool();
        if (m_buffer != VK_NULL_HANDLE)
        {
            memoryPool->destroyBuffer(m_buffer, m_bufferInfo);
        }
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        if(m_alignment > 0)
        {
            bufferInfo.size = ((ammount - 1) / m_alignment + 1) * m_alignment;
        }
        else
        {
            bufferInfo.size = ammount;
        }
        
        if(m_type == DeviceBufferType::Vertex){
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        
        }
        else if(m_type == DeviceBufferType::Index){
            bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        else if(m_type == DeviceBufferType::Uniform){
            bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto info = memoryPool->getBuffer(bufferInfo, &m_buffer);
        m_memory = info.getMemory();
        m_bufferSize = info.getSize();
        m_offset = info.getOffset();
        m_bufferInfo = info;
    }
	bool DeviceBufferVK::init(DeviceBufferType type)
	{
        m_type = type;
		return false;
	}
	void DeviceBufferVK::bind()
	{
	}
    bool DeviceBufferVK::hasEnoughRoom(size_t size)
    {
        return (getBuffer() != VK_NULL_HANDLE && this->getSize() > size);
    }
    bool DeviceBufferVK::isValid()
    {
        return getBuffer() != VK_NULL_HANDLE;
    }
    VkBuffer DeviceBufferVK::getBuffer()
    {
        return m_buffer;
    }
    size_t DeviceBufferVK::getSize()
    {
        return m_bufferSize;
    }
    size_t DeviceBufferVK::getOffset()
    {
        return m_offset;
    }
    VkDeviceMemory DeviceBufferVK::getMemory()
    {
        return m_memory;
    }

    void DeviceBufferVK::setUsePool(bool isUsed)
    {
        m_isUsePool = isUsed;
    }

    void DeviceBufferVK::map()
    {
        auto device = VKRenderBackEnd::shared()->getDevice();
        if(!m_isUsePool)
        {
            vkMapMemory(device, m_memory, m_offset, m_bufferSize, 0, &m_tmpData);
        }
        else
        {
            auto memoryPool = VKRenderBackEnd::shared()->getMemoryPool();
            memoryPool->map(m_bufferInfo, &m_tmpData);
        }
    }

    void DeviceBufferVK::unmap()
    {
        auto device = VKRenderBackEnd::shared()->getDevice();
        if(!m_isUsePool)
        {
            vkUnmapMemory(device, m_memory);
        }
        else
        {
            auto memoryPool = VKRenderBackEnd::shared()->getMemoryPool();
            memoryPool->unmap(m_bufferInfo);
        }
    }

    void DeviceBufferVK::copyFrom(void* ptr, size_t size, size_t memOffset)
    {
        memcpy((char *)m_tmpData + memOffset, ptr, size);
    }
 
}