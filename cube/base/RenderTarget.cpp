#include "RenderTarget.h"

RenderTarget::RenderTarget()
{
    m_GBuffer = new GBuffer();
    m_GBuffer->Init (1024,768);
    m_type = TargetType::OFF_SCREEN;
    m_resultBuffer =new RenderBuffer(1024,768);
}
GBuffer *RenderTarget::getGBuffer() const
{
    return m_GBuffer;
}

void RenderTarget::setGBuffer(GBuffer *GBuffer)
{
    m_GBuffer = GBuffer;
}
Camera *RenderTarget::camera() const
{
    return m_camera;
}

void RenderTarget::setCamera(Camera *camera)
{
    m_camera = camera;
}
RenderTarget::TargetType RenderTarget::type() const
{
    return m_type;
}

void RenderTarget::setType(const TargetType &type)
{
    m_type = type;
}
RenderBuffer *RenderTarget::resultBuffer() const
{
    return m_resultBuffer;
}

void RenderTarget::setResultBuffer(RenderBuffer *resultBuffer)
{
    m_resultBuffer = resultBuffer;
}

void RenderTarget::setIgnoreEntity(Entity *entity)
{
    m_ignoreList.push_back (entity);
}

bool RenderTarget::isIgnoreEntity(Entity *entity)
{
    if(m_ignoreList.empty ())
    {
        return false;
    }else
    {
        auto result = std::find(m_ignoreList.begin (),m_ignoreList.end (),entity);
        if(result!=m_ignoreList.end ())
        {
            return true;
        }else
        {
            return false;
        }
    }
}





