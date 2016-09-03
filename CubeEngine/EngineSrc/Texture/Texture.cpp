#include "Texture.h"
#include <QImage>
#include "External/SOIL/SOIL.h"
#include <QDebug>
namespace tzw {

Texture::Texture(std::string filePath)
{
    this->m_textureId =SOIL_load_OGL_texture (filePath.c_str(),0,0,SOIL_FLAG_INVERT_Y ,&m_width,&m_height);
    m_type = RenderFlag::TextureType::Texture2D;
    if (!m_textureId)
    {
        printf("texture create failed, no such file %s\n",filePath.c_str());
        exit(0);
    }
    setWarp(RenderFlag::WarpAddress::Repeat);
}

Texture::Texture(unsigned char *rawData, int w, int h, bool needFlipY)
{
    auto flags = 0;
    if (needFlipY)
    {
        flags = SOIL_FLAG_INVERT_Y;
    }
    this->m_textureId = SOIL_create_OGL_texture(rawData,w,h,2,0,flags);
    m_type = RenderFlag::TextureType::Texture2D;
    m_width = w;
    m_height = h;
    setWarp(RenderFlag::WarpAddress::Repeat);
}

Texture::Texture(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename)
{
    this->m_textureId = SOIL_load_OGL_cubemap(PosXFilename.c_str(),NegXFilename.c_str(),PosYFilename.c_str(),NegYFilename.c_str(),PosZFilename.c_str(),NegZFilename.c_str(),
                                              4,0,SOIL_FLAG_TEXTURE_REPEATS);
    qDebug()<< SOIL_last_result();
    m_type = RenderFlag::TextureType::TextureCubeMap;
}
/**
 * @brief Texture::setFilter
 * @param type 过滤类型
 * @param filter 指定过滤器,1代表min_filter,2代表mag_filter,0是两者皆设置
 */
void Texture::setFilter(Texture::FilterType type, int filter)
{
    switch(filter)
    {
    case 0:
        setMinFilter(type);
        setMagFilter(type);
        break;
    case 1:
        setMinFilter(type);
        break;
    case 2:
        setMagFilter(type);
        break;
    }
}

void Texture::setWarp(RenderFlag::WarpAddress warpAddress)
{
    RenderBackEnd::shared()->setTextureWarp(m_textureId,warpAddress,m_type);
}

unsigned int Texture::handle()
{
    return m_textureId;
}

vec2 Texture::getSize()
{
    return vec2(m_width,m_height);
}
RenderFlag::TextureType Texture::getType() const
{
    return m_type;
}


void Texture::setMinFilter(Texture::FilterType t)
{
    switch(t)
    {
    case FilterType::Linear:
        RenderBackEnd::shared()->setTexMIN(m_textureId,GL_LINEAR,m_type);
        break;
    case FilterType::Nearest:
        RenderBackEnd::shared()->setTexMIN(m_textureId,GL_NEAREST,m_type);
        break;
    }
}

void Texture::setMagFilter(Texture::FilterType t)
{
    switch(t)
    {
    case FilterType::Linear:
        RenderBackEnd::shared()->setTexMAG(m_textureId,GL_LINEAR,m_type);
        break;
    case FilterType::Nearest:
        RenderBackEnd::shared()->setTexMAG(m_textureId,GL_NEAREST,m_type);
        break;
    }
}

} // namespace tzw

