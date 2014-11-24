#include "pipeline.h"

PipeLine::PipeLine()
{
}

void PipeLine::setModelMatrix(QMatrix4x4 matrix)
{
    this->m_model = matrix;
}

void PipeLine::setViewMatrix(QMatrix4x4 matrix)
{
    this->m_view = matrix;
}

void PipeLine::setProjectionMatrix(QMatrix4x4 matrix)
{
    this->m_projection = matrix;
}

void PipeLine::setLightViewMatrix(QMatrix4x4 matrix)
{
    this->m_light_view = matrix;
}

void PipeLine::apply(ShaderProgram *shader)
{
    initializeOpenGLFunctions();
    QMatrix4x4 MVP = m_projection*m_view *m_model;
    QMatrix4x4 MV = m_view * m_model;
    shader->setUniformMat4v("MvpMatrix",MVP.data());
    shader->setUniformMat4v("modelViewMatrix",MV.data());
    shader->setUniformMat4v("ModelMatrix",m_model.data());
    shader->setUniformMat4v("Projection",m_projection.data());
}

void PipeLine::applyLightMvp(ShaderProgram *shader)
{
    initializeOpenGLFunctions();
    QMatrix4x4 MVP = m_projection*m_light_view *m_model;
    shader->setUniformMat4v("LightMvpMatrix",MVP.data());
}
