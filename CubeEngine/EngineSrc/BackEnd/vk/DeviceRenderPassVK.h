#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include <vector>
namespace tzw
{

struct DeviceRenderPassAttachmentInfo
{
	ImageFormat format;
	int attachmentIndex;
	bool isDepth;
};

class DeviceTextureVK;
class DeviceRenderPassVK
{
public:
	enum class OpType
	{
		LOADCLEAR_AND_STORE,
		LOAD_AND_STORE,
	};
	DeviceRenderPassVK(int colorAttachNum, OpType opType, ImageFormat format,bool isNeedTransitionToRread);
	VkRenderPass getRenderPass();
	size_t getAttachmentCount();
	std::vector<DeviceRenderPassAttachmentInfo> & getAttachmentList();
	OpType getOpType();
private:
	OpType m_opType;
	VkRenderPass m_renderPass;
	std::vector<DeviceRenderPassAttachmentInfo> m_attachmentList;

};
};

