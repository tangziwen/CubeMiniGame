#ifndef WATERPROJECTGRID_H
#define WATERPROJECTGRID_H
#include "base/node.h"
#include "entity.h"
#include <QOpenGLFunctions>
#include "geometry/mesh.h"
#include <QVector2D>
#include "base/RenderTarget.h"
#include <QMatrix4x4>

#include "waterNaive.h"

/*use project grid method to simulate water*/
class WaterProjectGrid : public Entity,protected QOpenGLFunctions
{
public:
    WaterProjectGrid(float width,float height,float waterLevel,float normal_splat = 15);
    float width() const;
    void setWidth(float width);

    float height() const;
    void setHeight(float height);

    virtual void adjustVertices();
    QVector3D caculateWorldPosUpper(QVector2D vec, QMatrix4x4 VP_R);
    QVector3D caculateWorldPosForLower(QVector2D vec, QMatrix4x4 VP_R);
    float waterLevel() const;
    void setWaterLevel(float waterLevel);

    QVector3D screenToWorld(QVector3D v, QMatrix4x4 VP_R);

    float normalSplatSize() const;
    void setNormalSplatSize(float normalSplatSize);

    float getLowerBound();
    float getUpperBound();
    float evaluateWave(Wave w, QVector2D vPos, float fTime);
    void adjustMinMax(float *min_x,float *min_y,float *max_x,float * max_y);
    TMesh *mesh() const;
    void setMesh(TMesh *mesh);

    RenderTarget *mirrorRenderTarget() const;
    void setMirrorRenderTarget(RenderTarget *mirrorRenderTarget);

private:
    void initWaves();
    RenderTarget * m_mirrorRenderTarget;
    float m_normalSplatSize;
    float m_width;
    float m_height;
    float m_waterLevel;
    TMesh * m_mesh;
    float m_time;
    Wave m_waves[4];
};

#endif // WATERPROJECTGRID_H
