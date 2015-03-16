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

void PipeLine::setEyePosition(QVector3D pos)
{
    m_eyePosition = pos;
}

void PipeLine::setEyeDirection(QVector3D dir)
{
    m_eyeDirection = dir;
    m_eyeDirection.normalize ();
}

void PipeLine::apply(ShaderProgram *shader)
{
    initializeOpenGLFunctions();
    QMatrix4x4 MVP = m_projection*m_view *m_model;
    QMatrix4x4 MV = m_view * m_model;
    shader->setUniformMat4v("g_MVP_matrix",MVP.data());
    shader->setUniformMat4v("g_MV_matrix",MV.data());
    shader->setUniformMat4v("g_model_matrix",m_model.data());
    shader->setUniformMat4v("g_projection_matrix",m_projection.data());
    shader->setUniform3Float ("g_eye_position",m_eyePosition.x (),m_eyePosition.y (),m_eyePosition.z ());
    shader->setUniform3Float ("g_eye_dir",m_eyeDirection.x (),m_eyeDirection.y (),m_eyeDirection.z ());
}

void PipeLine::applyLightMvp(ShaderProgram *shader)
{
    initializeOpenGLFunctions();
    QMatrix4x4 MVP = m_projection*m_light_view *m_model;
    shader->setUniformMat4v("g_light_MVP_matrix",MVP.data());
}
