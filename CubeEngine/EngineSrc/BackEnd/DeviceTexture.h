#pragma once


namespace tzw
{

struct ImageMetaInfo
{
	int channels;
	int width;
	int height;
	int dds_mipMapLevel;
};

class DeviceTexture
{
public:
	ImageMetaInfo m_metaInfo;
	unsigned int m_uid;//temporally for opengl
};


};

