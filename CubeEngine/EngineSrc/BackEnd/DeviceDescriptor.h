#pragma once
#include <vector>
#include <cstddef>
namespace tzw
{
class DeviceTexture;
class DeviceBuffer;
struct DeviceItemBuffer;
class DeviceDescriptor
{
public:
	DeviceDescriptor() = default;
	virtual ~DeviceDescriptor() = default;
	virtual void updateDescriptorByBinding(int binding, DeviceTexture * texture) = 0;
	virtual void updateDescriptorByBinding(int binding, std::vector<DeviceTexture *>& textureList) = 0;
	virtual void updateDescriptorByBinding(int binding, DeviceBuffer * buffer, size_t offset, size_t range) = 0;
	virtual void updateDescriptorByBinding(int binding, DeviceItemBuffer * itemBuff) = 0;
	// Copies data into backend-owned uniform storage valid for the current submission.
	virtual bool updateUniformByBinding(int binding, const void* data, size_t size) = 0;
	virtual void updateDescriptorByBindingAsStorageImage(int binding, DeviceTexture * texture) = 0;
protected:
};
};

