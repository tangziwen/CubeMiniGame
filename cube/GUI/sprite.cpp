#include "sprite.h"
#define FACTOR 100
Sprite::Sprite()
{
    vertices[0] = VertexData(QVector3D(0,1,-1),QVector2D(0,0));// tl
    vertices[1] = VertexData(QVector3D(0,0,-1),QVector2D(0,1));//bl
    vertices[2] = VertexData(QVector3D(1,0,-1),QVector2D(1,1));//br
    vertices[3] = VertexData(QVector3D(1,1,-1),QVector2D(1,0));//tr

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;
    initializeGLFunctions ();
    glGenBuffers (2,_vbo);
    glBindBuffer (GL_ARRAY_BUFFER,_vbo[0]);
    glBufferData (GL_ARRAY_BUFFER,4*sizeof(VertexData),vertices,GL_STREAM_DRAW);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER,_vbo[1]);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER,6*sizeof(GLushort),indices,GL_STATIC_DRAW);
    setNodeType (NODE_TYPE_SPRITE);
    onRender = NULL;
    setShader (ShaderPool::getInstance ()->get ("basic_sprite"));
}
Texture *Sprite::texture() const
{
    return m_texture;
}

void Sprite::setTexture(Texture *texture)
{
    m_texture = texture;
    updateVertices(texture->width (),texture->height ());
}

void Sprite::draw()
{
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D,m_texture->getTextureID ());
    this->shader ()->setUniformInteger ("g_diffuse_texture",0);
    GLuint shader_id = m_shader->getShaderId ();
    glUseProgram(shader_id);
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[1]);
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
    glDrawElements (GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
}
ShaderProgram *Sprite::shader() const
{
    return m_shader;
}

void Sprite::setShader(ShaderProgram *shader)
{
    m_shader = shader;
}
Camera *Sprite::camera() const
{
    return m_camera;
}

void Sprite::setCamera(Camera *camera)
{
    m_camera = camera;
}

void Sprite::updateVertices(float size_x,float size_y)
{
    vertices[0] = VertexData(QVector3D(0,size_y,-1),QVector2D(0,0));// tl
    vertices[1] = VertexData(QVector3D(0,0,-1),QVector2D(0,1));//bl
    vertices[2] = VertexData(QVector3D(size_x,0,-1),QVector2D(1,1));//br
    vertices[3] = VertexData(QVector3D(size_x,size_y,-1),QVector2D(1,0));//tr
    glBindBuffer (GL_ARRAY_BUFFER,_vbo[0]);
    glBufferData (GL_ARRAY_BUFFER,4*sizeof(VertexData),vertices,GL_STREAM_DRAW);
}




