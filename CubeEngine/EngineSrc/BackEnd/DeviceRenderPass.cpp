#include "DeviceRenderPass.h"

namespace tzw
{

	DeviceRenderPass::DeviceRenderPass()
	{
	}
	size_t DeviceRenderPass::getAttachmentCount()
	{
		return m_attachmentList.size();
	}
	std::vector<DeviceRenderPassAttachmentInfo>& DeviceRenderPass::getAttachmentList()
	{
		return m_attachmentList;
	}
	DeviceRenderPass::OpType DeviceRenderPass::getOpType()
	{
		return m_opType;
	}
}