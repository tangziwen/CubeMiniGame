#include "DeviceBufferVK.h"
#include "../VkRenderBackEnd.h"
namespace tzw
{
	void DeviceBufferVK::allocate(void* data, size_t ammount)
	{
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = ammount;
        if(m_type == DeviceBufferType::Vertex){
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        
        }
        else if(m_type == DeviceBufferType::Vertex){
            bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto device = VKRenderBackEnd::shared()->getDevice();
        vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer);

        VkMemoryRequirements memRequirements;
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
        VkDeviceMemory mem;
        vkAllocateMemory(device, &alloc_info, NULL,
                               &(mem));
        void* mapData;
        vkMapMemory(device, mem, 0, bufferInfo.size, 0, &mapData);
            memcpy(mapData, data, (size_t) bufferInfo.size);
        vkUnmapMemory(device, mem);
        vkBindBufferMemory(device, m_buffer, mem, 0);
	}

	bool DeviceBufferVK::init(DeviceBufferType type)
	{
        m_type = type;
		return false;
	}
	void DeviceBufferVK::bind()
	{
	}
    VkBuffer& DeviceBufferVK::getBuffer()
    {
        return m_buffer;
    }
}