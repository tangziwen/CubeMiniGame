#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H


#include <QOpenGLFunctions>
#include <QVector3D>
#include "shader/shader_program.h"
#include "baselight.h"
class DirectionalLight : public QOpenGLFunctions , public BaseLight
{
public:
    DirectionalLight();
    void setDirection(QVector3D new_direction);
    QVector3D getDirection();
    void setColor(QVector3D new_color);
    QVector3D getColor();
    void apply(ShaderProgram * shader);
    ShadowMapFBO * getCSM_FBO(int index);
private:
    ShadowMapFBO * m_CSM_FBO[4];
    QVector3D direction;
    QVector3D color;
};

#endif // DIRECTIONALLIGHT_H
