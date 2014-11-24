#ifndef BASELIGHT_H
#define BASELIGHT_H
#include <QVector3D>
#include "shader/shader_program.h"
class BaseLight
{
public:
    BaseLight();
    QVector3D getColor();
    void setColor(QVector3D new_color);
    float getIntensity();
    void setIntensity(float new_intensity);
    virtual void apply(ShaderProgram * program,int index);
protected:
    QVector3D color;
    float intensity;
};

#endif // BASELIGHT_H
