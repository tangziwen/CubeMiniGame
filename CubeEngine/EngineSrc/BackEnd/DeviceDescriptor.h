#pragma once
#include <vector>
namespace tzw
{
class DeviceTexture;
class DeviceBuffer;
struct DeviceItemBuffer;
class DeviceDescriptor
{
public:
	DeviceDescriptor() = default;
	virtual void updateDescriptorByBinding(int binding, DeviceTexture * texture) = 0;
	virtual void updateDescriptorByBinding(int binding, std::vector<DeviceTexture *>& textureList) = 0;
	virtual void updateDescriptorByBinding(int binding, DeviceBuffer * buffer, size_t offset, size_t range) = 0;
	virtual void updateDescriptorByBinding(int binding, DeviceItemBuffer * itemBuff) = 0;
protected:
};
};

