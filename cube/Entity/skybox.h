#ifndef SKYBOX_H
#define SKYBOX_H

#include "base/camera.h"
#include "geometry/mesh.h"
#include "shader/shader_program.h"
#include "Entity/Entity.h"
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
class SkyBox :protected QOpenGLFunctions
{
public:
    SkyBox(const char *  PosXFilename,
           const char *  NegXFilename,
           const char *  PosYFilename,
           const char *  NegYFilename,
           const char *  PosZFilename,
           const char *  NegZFilename);
    void Draw();
    Camera *camera() const;
    void setCamera(Camera *camera);
    TMesh *mesh() const;
    void setMesh(TMesh *mesh);
    ShaderProgram *shader() const;
    void setShader(ShaderProgram *shader);

    Entity *getEntity() const;
    void setEntity(Entity *value);

private:
    QOpenGLTexture * m_qTexture;
    Entity * entity;
    ShaderProgram * m_shader;
    TMesh * m_mesh;
    Camera * m_camera;
    unsigned int m_cubeMapTexture;
};

#endif // SKYBOX_H
