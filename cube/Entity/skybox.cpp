#include "skybox.h"
#include "external/SOIL/SOIL.h"
#include "material/materialpool.h"
SkyBox::SkyBox(const char *  PosXFilename,
               const char *  NegXFilename,
               const char *  PosYFilename,
               const char *  NegYFilename,
               const char *  PosZFilename,
               const char *  NegZFilename)
{
    initializeOpenGLFunctions();
    this->m_cubeMapTexture = SOIL_load_OGL_cubemap(PosXFilename,NegXFilename,PosYFilename,NegYFilename,PosZFilename,NegZFilename,4,0,SOIL_FLAG_TEXTURE_REPEATS);
    glBindTexture (GL_TEXTURE_3D,m_cubeMapTexture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    m_mesh = new TMesh();
    m_mesh->pushVertex (VertexData(QVector3D(-1,-1,1),QVector2D(0,0)));//0
    m_mesh->pushVertex (VertexData(QVector3D(1,-1,1),QVector2D(1,0)));//1
    m_mesh->pushVertex (VertexData(QVector3D(-1,1,1),QVector2D(0,1)));//2
    m_mesh->pushVertex (VertexData(QVector3D(1,1,1),QVector2D(1,1)));//3

    m_mesh->pushVertex (VertexData(QVector3D(-1,-1,-1),QVector2D(0,0)));//4
    m_mesh->pushVertex (VertexData(QVector3D(1,-1,-1),QVector2D(1,0)));//5
    m_mesh->pushVertex (VertexData(QVector3D(-1,1,-1),QVector2D(0,1)));//6
    m_mesh->pushVertex (VertexData(QVector3D(1,1,-1),QVector2D(1,1)));//7

    GLushort indices[]={2,0,1,2,1,3,3,1,5,3,5,7,7,5,4,7,4,6,6,4,0,6,0,2,6,2,3,6,3,7,5,1,0,5,0,4
    };
    m_mesh->pushIndex(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh->setMaterial (MaterialPool::getInstance ()->createMaterial ("default"));
    m_mesh->finishWithoutNormal();
    entity = new Entity();
    entity->addMesh(m_mesh);
    entity->scale(20,20,20);
    this->setShader(ShaderPoll::getInstance()->get("sky_box"));
}

void SkyBox::Draw()
{
    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP,this->m_cubeMapTexture);//bind cube map
    int shadowMap_ptr = glGetUniformLocation(m_shader->getShaderId(),"gCubemapTexture");
    glUniform1i(shadowMap_ptr,1);
    entity->draw();
    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);

}


Camera *SkyBox::camera() const
{
    return m_camera;
}

void SkyBox::setCamera(Camera *camera)
{
    m_camera = camera;
    entity->setCamera(camera);
}
TMesh *SkyBox::mesh() const
{
    return m_mesh;
}

void SkyBox::setMesh(TMesh *mesh)
{
    m_mesh = mesh;
}
ShaderProgram *SkyBox::shader() const
{
    return m_shader;
}

void SkyBox::setShader(ShaderProgram *shader)
{
    m_shader = shader;
    entity->setShaderProgram(shader);
}


Entity *SkyBox::getEntity() const
{
    return entity;
}

void SkyBox::setEntity(Entity *value)
{
    entity = value;
}




