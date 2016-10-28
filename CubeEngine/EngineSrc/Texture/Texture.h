#ifndef TZW_TEXTURE_H
#define TZW_TEXTURE_H

#include <string>
#include "../Math/vec2.h"
#include "../BackEnd/RenderBackEnd.h"
#include "../Rendering/RenderFlag.h"
namespace tzw {

class Texture
{
public:
    enum class FilterType
    {
        Linear,
        Nearest,
        LinearMipMapNearest,
    };
    Texture(std::string filePath);
    Texture(unsigned char * rawData,int w,int h,bool needFlipY = true);
    Texture(std::string  PosXFilename,
            std::string  NegXFilename,
            std::string  PosYFilename,
            std::string  NegYFilename,
            std::string  PosZFilename,
            std::string  NegZFilename);
    void setFilter(FilterType type, int filter = 0);
    void setWarp(RenderFlag::WarpAddress warpAddress);
    unsigned int handle();
    vec2 getSize();
    RenderFlag::TextureType getType() const;
    bool getIsHaveMipMap() const;
    void genMipMap();
    void initData();
private:
    RenderFlag::TextureType m_type;
    void setMinFilter(FilterType t);
    void setMagFilter(FilterType t);
    int m_width,m_height;
    unsigned short m_textureId;
    bool m_isHaveMipMap;
};

} // namespace tzw

#endif // TZW_TEXTURE_H
