#ifndef TZW_RENDERBACKEND_H
#define TZW_RENDERBACKEND_H
#include "../Rendering/RenderFlag.h"
namespace tzw {

class RenderBackEnd
{
public:
    static RenderBackEnd *shared();
    void initDevice();
    unsigned int genBuffer();
    void bindBuffer(RenderFlag::BufferTarget target,unsigned int handle);
    void uploadBufferData(RenderFlag::BufferTarget target, unsigned int size, const void * data);
    void activeTextureUnit(unsigned int id);
    void enableFunction(RenderFlag::RenderFunction state);
    void disableFunction(RenderFlag::RenderFunction state);
    void setTexMIN(unsigned int textureID, int param, RenderFlag::TextureType type);
    void setTexMAG(unsigned int textureID, int param, RenderFlag::TextureType type);
    void bindTexture2D(unsigned int texUnitID,unsigned int textureID,RenderFlag::TextureType type = RenderFlag::TextureType::Texture2D);
    void drawElement(RenderFlag::IndicesType type,unsigned int size, const void *indicesAddress);
    void setDepthTestMethod(const RenderFlag::DepthTestMethod & method);
    void setTextureWarp(unsigned int textureID, RenderFlag::WarpAddress warpAddress, RenderFlag::TextureType type);
private:
    static RenderBackEnd * m_instance;
    RenderBackEnd();
    //some state cache
    RenderFlag::DepthTestMethod m_depthTestMethodCache;
    unsigned int m_textureSlot[8];
};

} // namespace tzw

#endif // TZW_RENDERBACKEND_H
