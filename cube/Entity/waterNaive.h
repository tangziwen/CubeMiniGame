#ifndef WATER_H
#define WATER_H
#include "base/node.h"
#include "entity.h"
#include <QOpenGLFunctions>
#include "geometry/mesh.h"
#include <QVector2D>
#include "base/RenderTarget.h"


/*
 * the class of the water
 * It's a easy class used to simulate the small water area, It can be set everywhere.
 * but if you want to simulate huge water area , such as an ocean. you can use the waterPG(water projected grid implemention ) instead.
 */

struct Wave{
    float    fFreq;    // 频率 (2PI / 波长)
    float    fAmp;    // 振幅
    float    fPhase;    // 相伴 (速度 * 2PI / 波长)
    QVector2D    vDir;    // 方向
};

class Water : public Entity,protected QOpenGLFunctions
{
public:
    Water(float width, float height, float waterLevel = 10 ,float waterScale = 1);

    float width() const;
    void setWidth(float width);

    float height() const;
    void setHeight(float height);

    float waterScale() const;
    void setWaterScale(float waterScale);

    TMesh *mesh() const;
    void setMesh(TMesh *mesh);

    float waterLevel() const;
    void setWaterLevel(float waterLevel);

    float evaluateWave(Wave w, QVector2D vPos, float fTime);

    virtual void adjustVertices();

    RenderTarget *mirrorRenderTarget() const;
    void setMirrorRenderTarget(RenderTarget *mirrorRenderTarget);

    Camera *mirrorCamera() const;
    void setMirrorCamera(Camera *mirrorCamera);

private:
    void initWaves();
private:
    Camera * m_mirrorCamera;
    float m_time;
    Wave m_waves[4];
    TMesh * m_mesh;
    float m_width;
    float m_height;
    float m_waterScale;
    float m_waterLevel;
    RenderTarget * m_mirrorRenderTarget;
};
#endif // WATER_H
