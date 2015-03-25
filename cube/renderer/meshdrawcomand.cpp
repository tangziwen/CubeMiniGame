#include "meshdrawcomand.h"
#include "base/bonedata.h"
void MeshDrawComand::synchronizeData(ShaderProgram *shader, Material *material, GLuint vertices, GLuint Indices)
{
    if(this->material != material)
    {
        this->material = material;
    }
    if(this->vbo[0] != vertices)
    {
        this->vbo[0] = vertices;
    }
    if(this->vbo[1] != Indices)
    {
        this->vbo[1] = Indices;
    }
    if(this->shaderProgram != shader)
    {
        this->shaderProgram = shader;
        shader->use ();
    }
}

void MeshDrawComand::Init(Material* material, GLuint vertices, GLuint Indices, int indices_count)
{
    initializeOpenGLFunctions ();
    this->material=material;
    this->vbo[0]=vertices;
    this->vbo[1]=Indices;
    this->shaderProgram=NULL;
    this->indices_count = indices_count;
}

void MeshDrawComand::Init(Material *material, GLuint vertices, GLuint Indices, GLuint bone, int indices_count)
{
    initializeOpenGLFunctions ();
    this->material=material;
    this->vbo[0]=vertices;
    this->vbo[1]=Indices;
    this->vbo[2]=bone;
    this->shaderProgram=NULL;
    this->indices_count = indices_count;
}

void MeshDrawComand::CommandSetRenderState(ShaderProgram *shader, Material *material, GLuint vertices, GLuint Indices,bool withoutTexture)
{
    if(shader!= this->shaderProgram)
        setShaderState();
    if(!withoutTexture)
        setTextureState();
    if(vertices != this->vbo[0] || Indices != this->vbo[1])
        setAttribState();
}

void MeshDrawComand::setShaderState()
{
    this->shaderProgram->use();
    shader_id = this->shaderProgram->getShaderId();
}

MeshDrawComand::MeshDrawComand()
{
    initializeOpenGLFunctions ();
    for(int i =0;i<3;i++)
    {
        this->vbo[i]=0;
    }
}


void MeshDrawComand::Draw()
{
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, 0);
}

ShaderProgram *MeshDrawComand::getShaderProgram()
{
    return shaderProgram;
}

void MeshDrawComand::setTextureState()
{
    //diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->material->getDiffuse()->texture->getTextureID());
    this->shaderProgram->setUniformInteger ("g_diffuse_texture",0);

    //normal texture
    if(this->material->normalMap ())
    {
        glActiveTexture (GL_TEXTURE2);
        glBindTexture (GL_TEXTURE_2D,this->material->normalMap ()->getTextureID ());
        this->shaderProgram->setUniformInteger ("g_has_normal_map",1);
        this->shaderProgram->setUniformInteger ("g_normal_map",2);
    }else
    {
        this->shaderProgram->setUniformInteger ("g_has_normal_map",0);
        this->shaderProgram->setUniformInteger ("g_normal_map",0);
    }
}

void MeshDrawComand::setAttribState()
{
    glUseProgram(shader_id);
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    // Offset for position
    quintptr offset = offsetof(VertexData,position);
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = glGetAttribLocation (shader_id,"a_position");
    glEnableVertexAttribArray (vertexLocation);

    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    // Offset for texture coordinate
    offset = offsetof(VertexData,texCoord);
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = glGetAttribLocation (shader_id,"a_texcoord");
    if(texcoordLocation>=0)
    {
        glEnableVertexAttribArray(texcoordLocation);
        //pass the texcoord to shader
        glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    }

    offset =offsetof(VertexData,normalLine);
    int normalLineLocation = glGetAttribLocation (shader_id,"a_normal_line");
    if(normalLineLocation>=0)
    {
        glEnableVertexAttribArray(normalLineLocation);
        glVertexAttribPointer(normalLineLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    }
    offset = offsetof(VertexData,boneId);
    int BoneId = glGetAttribLocation (shader_id,"a_bone_ID"); // bone ID
    if(BoneId>=0)
    {
        glEnableVertexAttribArray(BoneId);
        glVertexAttribPointer(BoneId,4,GL_INT,GL_FALSE,sizeof(VertexData),(const void *)offset);
    }
     offset = offsetof(VertexData,boneWeight);
    int BoneWeight = glGetAttribLocation (shader_id,"a_bone_weight"); //bone weight
    if(BoneWeight>=0)
    {
        glEnableVertexAttribArray(BoneWeight);
        glVertexAttribIPointer(BoneWeight,4,GL_INT,sizeof(VertexData),(const void *)offset);
    }
    offset = offsetof(VertexData,tangent);
    int tangentLocation = glGetAttribLocation (shader_id,"a_tangent");
    if(tangentLocation >= 0)
    {
        glEnableVertexAttribArray (tangentLocation);
        glVertexAttribPointer (tangentLocation,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(const void *) offset);
    }
}
