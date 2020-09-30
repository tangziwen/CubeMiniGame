#pragma once
#include "Rendering/ImageFormat.h"
#include "DeviceRenderPass.h"
#include <vector>
#include "Math/vec4.h"
namespace tzw
{

class DeviceTexture;
class DeviceFrameBuffer
{
public:
	DeviceFrameBuffer() = default;
	virtual void init(int w, int h, DeviceRenderPass * renderPass) = 0;
	virtual void init(DeviceTexture * tex, DeviceTexture * depth, DeviceRenderPass * renderPass) = 0;
	virtual DeviceTexture * getDepthMap() = 0;
	virtual std::vector<DeviceTexture *> & getTextureList() = 0;
	virtual vec2 getSize() = 0;
protected:
	std::vector<DeviceTexture *>m_textureList;
	DeviceTexture * m_depthTexture;
	vec2 m_size;
};
};

