#pragma once
#include "Rendering/ImageFormat.h"
#include <vector>
namespace tzw
{

struct DeviceRenderPassAttachmentInfo
{
	ImageFormat format;
	bool isDepthStencilAttachment;
};
typedef std::vector<DeviceRenderPassAttachmentInfo> DeviceAttachmentInfoList;

typedef std::vector<ImageFormat> colorAttachmentFormatList;
class DeviceRenderPass
{
public:
	enum class OpType
	{
		LOADCLEAR_AND_STORE,
		LOAD_AND_STORE,
	};
	DeviceRenderPass();
	virtual void init(const DeviceAttachmentInfoList & attachList, OpType opType,bool isNeedTransitionToRread, bool isOutputToScreen = false) = 0;
	size_t getAttachmentCount();
	std::vector<DeviceRenderPassAttachmentInfo> & getAttachmentList();
	OpType getOpType();
protected:
	OpType m_opType;
	bool m_isNeedTransitionToRead;
	bool m_isOutPutToScreen;
	std::vector<DeviceRenderPassAttachmentInfo> m_attachmentList;

};

};

