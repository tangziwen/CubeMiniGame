#pragma once
#include <cstdint>

#include "Math/vec2.h"

namespace tzw
{

class DeviceTexture;

enum class DeviceTextureLayout
{
	Unknown,
	Undefined,
	ColorAttachment,
	DepthAttachment,
	DepthRead,
	ShaderRead,
	TransferSrc,
	TransferDst,
	General,
	Present,
};

enum class DeviceTextureUsage
{
	Unknown,
	None,
	FragmentShaderRead,
	ComputeStorageRead,
	ComputeStorageWrite,
	ComputeStorageReadWrite,
	ColorAttachmentWrite,
	DepthAttachmentWrite,
	TransferRead,
	TransferWrite,
	Present,
};

struct DeviceTextureState
{
	DeviceTextureLayout layout = DeviceTextureLayout::Unknown;
	DeviceTextureUsage usage = DeviceTextureUsage::Unknown;
};

struct DeviceTextureBarrier
{
	DeviceTexture* texture = nullptr;
	DeviceTextureState before;
	DeviceTextureState after;
	uint32_t baseMipLevel = 0;
	uint32_t levelCount = 1;
};

struct DeviceTextureBlit
{
	DeviceTexture* source = nullptr;
	DeviceTexture* destination = nullptr;
	vec2 size = vec2(0, 0);
	uint32_t sourceMipLevel = 0;
	uint32_t destinationMipLevel = 0;
};

class DeviceRenderCommand
{
public:
	DeviceRenderCommand(void * handle);
	virtual ~DeviceRenderCommand() = default;
	virtual void startRecord() = 0;
	virtual void endRecord() = 0;
	virtual bool textureBarrier(const DeviceTextureBarrier& barrier) = 0;
	virtual bool blitTexture(const DeviceTextureBlit& blit) = 0;
	void * get();
protected:
	void * m_handle;
};


};

