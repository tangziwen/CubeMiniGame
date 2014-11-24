#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include <QOpenGLFunctions>
#include "light/pointlight.h"
#include "shader/shader_program.h"
class SpotLight : public PointLight
{
public:
    SpotLight();
    void setAngle(float new_angle);
    float getAngle();
    void setOutterAngle(float new_angle);
    float getOutterAngle();
    void apply(ShaderProgram * shader, int index);
    void setDirection(QVector3D new_direction);
    QVector3D getDirection();
private:
    QVector3D direction;
    float angle;
    float outterAngle;
};

#endif // SPOTLIGHT_H
