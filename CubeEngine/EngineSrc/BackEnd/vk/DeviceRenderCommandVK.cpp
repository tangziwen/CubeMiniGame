#include "DeviceRenderCommandVK.h"

#include "BackEnd/vk/DeviceTextureVK.h"
#include "Utility/log/Log.h"

#include <cstdlib>

namespace tzw
{
namespace
{
bool toVkLayout(DeviceTextureLayout layout, VkImageLayout& outLayout)
{
	switch(layout)
	{
	case DeviceTextureLayout::ColorAttachment:
		outLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		return true;
	case DeviceTextureLayout::DepthAttachment:
		outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		return true;
	case DeviceTextureLayout::DepthRead:
		outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		return true;
	case DeviceTextureLayout::ShaderRead:
		outLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		return true;
	case DeviceTextureLayout::TransferSrc:
		outLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		return true;
	case DeviceTextureLayout::TransferDst:
		outLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		return true;
	case DeviceTextureLayout::General:
		outLayout = VK_IMAGE_LAYOUT_GENERAL;
		return true;
	case DeviceTextureLayout::Present:
		outLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return true;
	case DeviceTextureLayout::Unknown:
		break;
	}
	return false;
}

bool toVkState(const DeviceTextureState& state, VkImageLayout& layout, VkPipelineStageFlags& stage, VkAccessFlags& access)
{
	if(!toVkLayout(state.layout, layout))
	{
		return false;
	}

	switch(state.usage)
	{
	case DeviceTextureUsage::FragmentShaderRead:
		stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		access = VK_ACCESS_SHADER_READ_BIT;
		return true;
	case DeviceTextureUsage::ComputeStorageRead:
		stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		access = VK_ACCESS_SHADER_READ_BIT;
		return true;
	case DeviceTextureUsage::ComputeStorageWrite:
		stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		access = VK_ACCESS_SHADER_WRITE_BIT;
		return true;
	case DeviceTextureUsage::ComputeStorageReadWrite:
		stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		access = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		return true;
	case DeviceTextureUsage::ColorAttachmentWrite:
		stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		return true;
	case DeviceTextureUsage::DepthAttachmentWrite:
		stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		return true;
	case DeviceTextureUsage::TransferRead:
		stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		access = VK_ACCESS_TRANSFER_READ_BIT;
		return true;
	case DeviceTextureUsage::TransferWrite:
		stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		access = VK_ACCESS_TRANSFER_WRITE_BIT;
		return true;
	case DeviceTextureUsage::Present:
		stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		access = 0;
		return true;
	case DeviceTextureUsage::Unknown:
		break;
	}
	return false;
}
}

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

bool DeviceRenderCommandVK::textureBarrier(const DeviceTextureBarrier& barrier)
{
	auto texture = dynamic_cast<DeviceTextureVK*>(barrier.texture);
	if(!texture || barrier.levelCount == 0)
	{
		tlogError("DeviceRenderCommandVK textureBarrier received an invalid texture or mip range.");
		return false;
	}

	VkImageLayout oldLayout;
	VkImageLayout newLayout;
	VkPipelineStageFlags srcStage = {};
	VkPipelineStageFlags dstStage = {};
	VkAccessFlags srcAccess = {};
	VkAccessFlags dstAccess = {};
	if(!toVkState(barrier.before, oldLayout, srcStage, srcAccess)
		|| !toVkState(barrier.after, newLayout, dstStage, dstAccess))
	{
		tlogError("DeviceRenderCommandVK textureBarrier received an unsupported state.");
		return false;
	}

	VkImageMemoryBarrier imageBarrier{};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.oldLayout = oldLayout;
	imageBarrier.newLayout = newLayout;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = texture->getImage();
	imageBarrier.subresourceRange.aspectMask = texture->getImageAspectFlag();
	imageBarrier.subresourceRange.baseMipLevel = barrier.baseMipLevel;
	imageBarrier.subresourceRange.levelCount = barrier.levelCount;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.layerCount = 1;
	imageBarrier.srcAccessMask = srcAccess;
	imageBarrier.dstAccessMask = dstAccess;

	vkCmdPipelineBarrier(getVK(), srcStage, dstStage, 0,
		0, nullptr, 0, nullptr, 1, &imageBarrier);
	return true;
}

bool DeviceRenderCommandVK::blitTexture(const DeviceTextureBlit& blit)
{
	auto source = dynamic_cast<DeviceTextureVK*>(blit.source);
	auto destination = dynamic_cast<DeviceTextureVK*>(blit.destination);
	if(!source || !destination || blit.size.x <= 0 || blit.size.y <= 0
		|| source->getTextureRole() != destination->getTextureRole())
	{
		tlogError("DeviceRenderCommandVK blitTexture received incompatible resources.");
		return false;
	}

	VkImageBlit region{};
	region.srcOffsets[0] = {0, 0, 0};
	region.srcOffsets[1] = {static_cast<int32_t>(blit.size.x), static_cast<int32_t>(blit.size.y), 1};
	region.srcSubresource.aspectMask = source->getImageAspectFlag();
	region.srcSubresource.mipLevel = blit.sourceMipLevel;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount = 1;
	region.dstOffsets[0] = {0, 0, 0};
	region.dstOffsets[1] = {static_cast<int32_t>(blit.size.x), static_cast<int32_t>(blit.size.y), 1};
	region.dstSubresource.aspectMask = destination->getImageAspectFlag();
	region.dstSubresource.mipLevel = blit.destinationMipLevel;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount = 1;

	vkCmdBlitImage(getVK(), source->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region, VK_FILTER_NEAREST);
	return true;
}

}
