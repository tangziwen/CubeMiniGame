#ifndef MESHDRAWCOMAND_H
#define MESHDRAWCOMAND_H
#include <QOpenGLFunctions_3_0>
#include <QGLShaderProgram>
#include "base/vertexdata.h"
#include "shader/shader_program.h"
#include <QMatrix4x4>
#include "material/material.h"
#include "base/bonedata.h"
class MeshDrawComand : protected QOpenGLFunctions_3_0
{
public:
    void synchronizeData(ShaderProgram * shader, Material *material, GLuint vertices , GLuint Indices );
    void Init(Material *material, GLuint vertices , GLuint Indices , int indices_count);
    void Init(Material *material, GLuint vertices , GLuint Indices ,GLuint bone, int indices_count);
    void CommandSetRenderState(ShaderProgram *shader, Material *material, GLuint vertices, GLuint Indices, bool withoutTexture =false);
    void setShaderState();
    MeshDrawComand();
    void Draw();
    ShaderProgram *getShaderProgram();
    std::vector<BoneData> m_bones;
private:
    int indices_count;
    void setTextureState();
    void setAttribState();
    ShaderProgram * shaderProgram;
    Material * material;
    GLuint vbo[3];
    int shader_id;
};

#endif // MESHDRAWCOMAND_H
