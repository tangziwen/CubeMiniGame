#pragma once
#include "Rendering/ImageFormat.h"
#include <cstdint>

namespace tzw
{


enum class TextureRoleEnum
{
	AS_COLOR,
	AS_DEPTH,
};
	
enum class TextureUsageEnum
{
	SAMPLE_ONLY,
	SAMPLE_AND_ATTACHMENT,
	
};
	
struct DeviceTextureDesc
{
    uint32_t width = 0;
    uint32_t height = 0;
    ImageFormat format = ImageFormat::RGBA8_UNorm;
    TextureRoleEnum role = TextureRoleEnum::AS_COLOR;
    TextureUsageEnum usage = TextureUsageEnum::SAMPLE_AND_ATTACHMENT;
};

struct ImageMetaInfo
{
	int channels;
	int width;
	int height;
	int dds_mipMapLevel;
	ImageFormat m_imageFormat;
};



class DeviceTexture
{
public:
	virtual ~DeviceTexture() = default;

	ImageMetaInfo m_metaInfo = {};
	unsigned int m_uid = 0;//temporally for opengl
	[[nodiscard]] TextureRoleEnum getTextureRole() const
	{
		return m_textureRole;
	}

	void setTextureRole(TextureRoleEnum textureUsage)
	{
		m_textureRole = textureUsage;
	}
	
	[[nodiscard]] TextureUsageEnum getTextureUsage() const
	{
		return m_textureUsage;
	}

	void setTextureUsage(TextureUsageEnum textureUsage)
	{
		m_textureUsage = textureUsage;
	}
protected:
	TextureRoleEnum m_textureRole = TextureRoleEnum::AS_COLOR;
	TextureUsageEnum m_textureUsage = TextureUsageEnum::SAMPLE_ONLY;
};


};

