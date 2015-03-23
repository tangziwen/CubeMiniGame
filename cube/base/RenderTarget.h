#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "renderer/gbuffer.h"
#include "base/camera.h"
#include "renderer/renderbuffer.h"
#include "Entity/entity.h"
#include <vector>
class RenderTarget
{
public:
    enum class TargetType{
        ON_SCREEN,
        OFF_SCREEN,
    };
    RenderTarget();

    GBuffer *getGBuffer() const;
    void setGBuffer(GBuffer *GBuffer);
    Camera *camera() const;
    void setCamera(Camera *camera);
    TargetType type() const;
    void setType(const TargetType &type);

    RenderBuffer *resultBuffer() const;
    void setResultBuffer(RenderBuffer *resultBuffer);

    void setIgnoreEntity(Entity * entity);

    bool isIgnoreEntity(Entity * entity);

    QMatrix4x4 auxMatrix() const;
    void setAuxMatrix(const QMatrix4x4 &auxMatrix);

private:
    QMatrix4x4 m_auxMatrix;
    TargetType m_type;
    Camera * m_camera;
    GBuffer * m_GBuffer;
    RenderBuffer * m_resultBuffer;
    std::vector<Entity*> m_ignoreList;

};

#endif // RENDERTARGET_H
