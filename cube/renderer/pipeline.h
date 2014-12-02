#ifndef PIPELINE_H
#define PIPELINE_H
#include <QMatrix4x4>
#include <qopenglfunctions.h>
#include "shader/shader_program.h"
class PipeLine :protected QOpenGLFunctions
{
public:
    PipeLine();
    void setModelMatrix(QMatrix4x4 matrix);
    void setViewMatrix(QMatrix4x4 matrix);
    void setProjectionMatrix(QMatrix4x4 matrix);
    void setLightViewMatrix(QMatrix4x4 matrix);
    void setEyePosition(QVector3D pos);
    void apply(ShaderProgram *shader);
    void applyLightMvp(ShaderProgram * shader);
private:
    QMatrix4x4 m_light_view;
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QVector3D m_eyePosition;
};

#endif // PIPELINE_H
