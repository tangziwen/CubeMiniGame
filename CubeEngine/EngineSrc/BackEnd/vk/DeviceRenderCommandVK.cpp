#include "DeviceRenderCommandVK.h"

#include <cstdlib>

namespace tzw
{
DeviceRenderCommandVK::DeviceRenderCommandVK(VkCommandBuffer handle)
	:DeviceRenderCommand(handle)
{
}

void DeviceRenderCommandVK::startRecord()
{
	VkCommandBufferBeginInfo beginInfoDeffered = {};
    beginInfoDeffered.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfoDeffered.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    
    int res = vkBeginCommandBuffer(getVK(), &beginInfoDeffered);
	if(res != VK_SUCCESS)
	{
		abort();
	}
}

void DeviceRenderCommandVK::endRecord()
{
    vkEndCommandBuffer(getVK());
}


}
