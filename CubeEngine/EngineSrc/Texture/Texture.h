#ifndef TZW_TEXTURE_H
#define TZW_TEXTURE_H

#include <string>
#include "../Math/vec2.h"
#include "../Rendering/RenderFlag.h"
#include <functional>
#include "BackEnd/DeviceTexture.h"
namespace tzw {
class Texture
{
public:
    enum class FilterType
    {
        Linear,
        Nearest,
        LinearMipMapNearest,
		LinearMipMapLinear,
		NearestMipMapLinear,
    };
	Texture();
    Texture(std::string filePath);
	Texture(std::string filePath, char faceMode[6]);
    Texture(unsigned char * rawData,int w,int h,ImageFormat format, bool needFlipY = true);
    Texture(std::string  PosXFilename,
            std::string  NegXFilename,
            std::string  PosYFilename,
            std::string  NegYFilename,
            std::string  PosZFilename,
            std::string  NegZFilename);
    void setFilter(FilterType type, int filter = 0);
    void setWarp(RenderFlag::WarpAddress warpAddress);
	void loadAsync(std::string filePath, std::function<void(Texture *)> onFinish);
    DeviceTexture* handle();
    vec2 getSize();
    RenderFlag::TextureType getType() const;
    bool getIsHaveMipMap() const;
    void genMipMap();
    void initData();
	DeviceTexture * getTextureId() const;
	void setTextureId(DeviceTexture * textureId);
    std::string getFilePath(){return m_filePath;}
private:
    RenderFlag::TextureType m_type;
    void setMinFilter(FilterType t);
    void setMagFilter(FilterType t);
    int m_width,m_height;
    DeviceTexture * m_textureId;
    bool m_isHaveMipMap;
	unsigned char *m_imgData;
	int m_channel;
	bool m_isLoaded;
    std::string m_filePath;
};

} // namespace tzw

#endif // TZW_TEXTURE_H
