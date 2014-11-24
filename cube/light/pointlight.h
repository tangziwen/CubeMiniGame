#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <QVector3D>
#include <QOpenGLFunctions>
#include "shader/shader_program.h"
#include "baselight.h"
class PointLight : public QOpenGLFunctions , public BaseLight
{
public:
    PointLight();
    QVector3D getColor();
    void setColor(QVector3D new_color);
    QVector3D getPos();
    void setPos(QVector3D new_pos);
    void apply(ShaderProgram * shader,int index);
    float getRange();
    void setRange(float new_attenuation);
protected:
    float range;
    QVector3D color;
    QVector3D pos;
};

#endif // POINTLIGHT_H
