#include "DeviceItemBufferPoolVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceMemoryPoolVK.h"
#define VMA_IMPLEMENTATION
#include "External/vulkan/vk_mem_alloc.h"
namespace tzw
{
	static VmaAllocator g_allocator;
	DeviceMemoryPoolVK::DeviceMemoryPoolVK(VkPhysicalDevice phyDevice, VkDevice device, VkInstance inst)
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = phyDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = inst;
		vmaCreateAllocator(&allocatorInfo, &g_allocator);
	}

	BufferInfo DeviceMemoryPoolVK::createBuffer(VkBufferCreateInfo bufferCreateInfo, VkBuffer* targetBuffer)
	{
		BufferInfo info{};
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
 
		VmaAllocation allocation;
		auto result = vmaCreateBuffer(g_allocator, &bufferCreateInfo, &allocInfo, targetBuffer, &allocation, nullptr);
		if(result < VK_SUCCESS)
		{
			abort();
		}
		info.m_ptr = allocation;
		return info;
	}

	void DeviceMemoryPoolVK::destroyBuffer(VkBuffer targetBuffer, BufferInfo info)
	{
		vmaDestroyBuffer(g_allocator, targetBuffer, static_cast<VmaAllocation>(info.m_ptr));
	}

	void DeviceMemoryPoolVK::map(BufferInfo info, void ** mappedData)
	{
		auto result = vmaMapMemory(g_allocator, static_cast<VmaAllocation>(info.m_ptr), mappedData);
		if(result != VK_SUCCESS)
		{
			abort();
		}
	}

	void DeviceMemoryPoolVK::unmap(BufferInfo info)
	{
		vmaUnmapMemory(g_allocator, static_cast<VmaAllocation>(info.m_ptr));
	}

	size_t BufferInfo::getSize()
	{
		VmaAllocationInfo info;
		vmaGetAllocationInfo(g_allocator, static_cast<VmaAllocation>(m_ptr), &info);
		return info.size;
	}
	size_t BufferInfo::getOffset()
	{
		VmaAllocationInfo info;
		vmaGetAllocationInfo(g_allocator, static_cast<VmaAllocation>(m_ptr), &info);
		return info.offset;
	}
	VkDeviceMemory BufferInfo::getMemory()
	{
		VmaAllocationInfo info;
		vmaGetAllocationInfo(g_allocator, static_cast<VmaAllocation>(m_ptr), &info);
		return info.deviceMemory;
	}
}