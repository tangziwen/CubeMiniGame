#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H
#include <QVector3D>
#include <QOpenGLFunctions>
#include "shader/shader_program.h"
#include "baselight.h"
class AmbientLight :  public QOpenGLFunctions , public BaseLight
{
public:
    AmbientLight();
    void setColor(QVector3D new_color);
    QVector3D getColor();
    void apply(ShaderProgram * shader);
private:
    QVector3D color;
};

#endif // AMBIENTLIGHT_H
