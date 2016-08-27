#include "RenderBackEnd.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
namespace tzw {
RenderBackEnd * RenderBackEnd::m_instance = nullptr;
RenderBackEnd *RenderBackEnd::shared()
{
    if(!m_instance)
    {
        m_instance = new RenderBackEnd();

    }

    return m_instance;
}

void RenderBackEnd::initDevice()
{
    auto result = glewInit();
    if(result != GLEW_OK)
    {
        printf("opengl device init error!\n");
        exit(0);
    }

    // Set the background color
    glClearColor(0, 0, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int RenderBackEnd::genBuffer()
{
    unsigned int handle;
    glGenBuffers(1,&handle);
    return handle;
}

void RenderBackEnd::bindBuffer(RenderFlag::BufferTarget target, unsigned int handle)
{
    switch(target)
    {
    case RenderFlag::BufferTarget::VertexBuffer:
        glBindBuffer(GL_ARRAY_BUFFER,handle);
        break;
    case RenderFlag::BufferTarget::IndexBuffer:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,handle);
        break;
    default:

        break;
    }
}

void RenderBackEnd::uploadBufferData(RenderFlag::BufferTarget target, unsigned int size, const void *data)
{
    switch(target)
    {
    case RenderFlag::BufferTarget::VertexBuffer:
        glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
        break;
    case RenderFlag::BufferTarget::IndexBuffer:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
        break;
    default:
        break;
    }
}

void RenderBackEnd::activeTextureUnit(unsigned int id)
{
    glActiveTexture(GL_TEXTURE0 + id);
}

void RenderBackEnd::enableFunction(RenderFlag::RenderFunction state)
{
    switch(state)
    {
    case RenderFlag::RenderFunction::DepthTest:
        glEnable(GL_DEPTH_TEST);
        break;
    case RenderFlag::RenderFunction::AlphaBlend:
        glEnable(GL_BLEND);
        break;
    }
}

void RenderBackEnd::disableFunction(RenderFlag::RenderFunction state)
{
    switch(state)
    {
    case RenderFlag::RenderFunction::DepthTest:
        glDisable(GL_DEPTH_TEST);
        break;
    case RenderFlag::RenderFunction::AlphaBlend:
        glDisable(GL_BLEND);
        break;
    }
}

void RenderBackEnd::setTexMIN(unsigned int textureID, int param, RenderFlag::TextureType type)
{
    bindTexture2D(0,textureID,type);
    switch (type) {
    case RenderFlag::TextureType::Texture2D:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
        break;
    case RenderFlag::TextureType::TextureCubeMap:
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
        break;
    default:
        break;
    }
}

void RenderBackEnd::setTexMAG(unsigned int textureID, int param, RenderFlag::TextureType type)
{
    bindTexture2D(0,textureID,type);
    switch (type) {
    case RenderFlag::TextureType::Texture2D:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
        break;
    case RenderFlag::TextureType::TextureCubeMap:
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);
        break;
    default:
        break;
    }
}

void RenderBackEnd::bindTexture2D(unsigned int texUnitID, unsigned int textureID, RenderFlag::TextureType type)
{
    if (m_textureSlot[texUnitID] == textureID) return;
    m_textureSlot[texUnitID] = textureID;
    glActiveTexture(GL_TEXTURE0 + texUnitID);
    switch(type)
    {
    case RenderFlag::TextureType::Texture2D:
        glBindTexture(GL_TEXTURE_2D,textureID);
        break;
    case RenderFlag::TextureType::TextureCubeMap:
        glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
        break;
    }
}

void RenderBackEnd::drawElement(RenderFlag::IndicesType type, unsigned int size, const void * indicesAddress)
{
    switch(type)
    {
    case RenderFlag::IndicesType::Triangles:
        glDrawElements(GL_TRIANGLES,size, GL_UNSIGNED_SHORT, indicesAddress);
        break;
    case RenderFlag::IndicesType::TriangleStrip:
        glDrawElements(GL_TRIANGLE_STRIP, size, GL_UNSIGNED_SHORT, indicesAddress);
        break;
    }
}

void RenderBackEnd::setDepthTestMethod(const RenderFlag::DepthTestMethod &method)
{
    if(m_depthTestMethodCache == method) return;
    m_depthTestMethodCache = method;
    switch(method)
    {
    case RenderFlag::DepthTestMethod::Less:
        glDepthFunc(GL_LESS);
        break;
    case RenderFlag::DepthTestMethod::LessOrEqual:
        glDepthFunc(GL_LEQUAL);
        break;
    }
}

void RenderBackEnd::setTextureWarp(unsigned int textureID, RenderFlag::WarpAddress warpAddress, RenderFlag::TextureType type)
{
    bindTexture2D(0,textureID,type);
    unsigned int flag = 0;
    switch (warpAddress) {
    case RenderFlag::WarpAddress::Clamp:
        flag = GL_CLAMP;
        break;
    case RenderFlag::WarpAddress::Repeat:
        flag = GL_REPEAT;
        break;
    case RenderFlag::WarpAddress::ClampToEdge:
        flag = GL_CLAMP_TO_EDGE;
        break;
    default:
        break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, flag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, flag);
}

void RenderBackEnd::bindFrameBuffer(unsigned int frameBufferID)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
}

void RenderBackEnd::blitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1)
{
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void RenderBackEnd::setDepthTestEnable(bool isEnable)
{
    if(isEnable){
        glEnable(GL_DEPTH_TEST);
    }
    else{
        glDisable(GL_DEPTH_TEST);
    }
}

void RenderBackEnd::setDepthMaskWriteEnable(bool isEnable)
{
    if(isEnable)
    {
        glDepthMask(GL_TRUE);
    }else
    {
        glDepthMask(GL_FALSE);
    }
}

RenderBackEnd::RenderBackEnd()
{
    memset(m_textureSlot,0,sizeof(m_textureSlot));
    m_depthTestMethodCache = RenderFlag::DepthTestMethod::Less;
}

} // namespace tzw

