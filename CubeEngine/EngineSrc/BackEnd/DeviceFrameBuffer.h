#pragma once
#include "Rendering/ImageFormat.h"

#include <vector>
namespace tzw
{

class DeviceTexture;
class DeviceFrameBuffer
{
public:
	DeviceFrameBuffer();
	DeviceTexture * getDepthMap();
	std::vector<DeviceTexture *> & getTextureList();
	vec2 getSize();
private:
	std::vector<DeviceTextureVK *>m_textureList;
	DeviceTextureVK * m_depthTexture;
	vec2 m_size;
};
};

