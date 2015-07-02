#include "sprite.h"

Sprite::Sprite()
{
    m_vertices[0] = VertexData(QVector3D(0,1,-1),QVector2D(0,0));// tl
    m_vertices[1] = VertexData(QVector3D(0,0,-1),QVector2D(0,1));//bl
    m_vertices[2] = VertexData(QVector3D(1,0,-1),QVector2D(1,1));//br
    m_vertices[3] = VertexData(QVector3D(1,1,-1),QVector2D(1,0));//tr

    m_indices[0] = 0;
    m_indices[1] = 1;
    m_indices[2] = 2;

    m_indices[3] = 0;
    m_indices[4] = 2;
    m_indices[5] = 3;
    initializeGLFunctions ();
    glGenBuffers (2,_vbo);
    glBindBuffer (GL_ARRAY_BUFFER,_vbo[0]);
    glBufferData (GL_ARRAY_BUFFER,4*sizeof(VertexData),m_vertices,GL_STREAM_DRAW);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER,_vbo[1]);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER,6*sizeof(GLushort),m_indices,GL_STATIC_DRAW);
    setNodeType (NODE_TYPE_SPRITE);
    onRender = nullptr;
    setShader (ShaderPool::getInstance ()->get ("basic_sprite"));
}

Texture *Sprite::texture() const
{
    return m_texture;
}

void Sprite::setTexture(Texture *texture)
{
    m_texture = texture;
    setRect(QVector2D(0,0),QVector2D(1,1),texture->width (),texture->height ());
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

void Sprite::setRect(QVector2D TL, QVector2D BR, float the_size_x,float the_size_y)
{
    m_vertices[0] = VertexData(QVector3D(0,the_size_y,-1),TL);// tl
    m_vertices[1] = VertexData(QVector3D(0,0,-1),QVector2D(TL.x (),BR.y ()));//bl
    m_vertices[2] = VertexData(QVector3D(the_size_x,0,-1),BR);//br
    m_vertices[3] = VertexData(QVector3D(the_size_x,the_size_y,-1),QVector2D(BR.x (),TL.y ()));//tr
    glBindBuffer (GL_ARRAY_BUFFER,_vbo[0]);
    glBufferData (GL_ARRAY_BUFFER,4*sizeof(VertexData),m_vertices,GL_STREAM_DRAW);

    m_sizeX = the_size_x;
    m_sizeY = the_size_y;
    m_TL = TL;
    m_BR = BR;
}

void Sprite::setRect(QVector2D TL, QVector2D BR)
{
    setRect(TL,BR,m_sizeX,m_sizeY);
}

QVector2D Sprite::getSize()
{
    return QVector2D(m_sizeX,m_sizeY);
}

void Sprite::setSize(int width, int height)
{
    setRect (m_TL,m_BR,width,height);
}
QVector2D Sprite::TL() const
{
    return m_TL;
}

void Sprite::setTL(const QVector2D &TL)
{
    m_TL = TL;
}
QVector2D Sprite::BR() const
{
    return m_BR;
}

void Sprite::setBR(const QVector2D &BR)
{
    m_BR = BR;
}


