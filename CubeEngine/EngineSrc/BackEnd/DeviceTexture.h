#pragma once
#include "Rendering/ImageFormat.h"

namespace tzw
{

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
	ImageMetaInfo m_metaInfo;
	unsigned int m_uid;//temporally for opengl
};


};

