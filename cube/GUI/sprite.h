#ifndef SPRITE_H
#define SPRITE_H
#include "shader/shaderpoll.h"
#include "texture/texturepool.h"
#include "base/node.h"
#include "base/vertexdata.h"
#include "base/camera.h"
#include <vector>
#include <QGLFunctions>
//Sprite class , use to handle GUI element
class Sprite : public Node, QGLFunctions
{
public:
    Sprite();
    Texture *texture() const;
    void setTexture(Texture *texture);
    void draw();
    ShaderProgram *shader() const;
    void setShader(ShaderProgram *shader);
    void (*onRender)(Sprite * self,float dt);
    Camera *camera() const;
    void setCamera(Camera *camera);
    void setRect(QVector2D TL,QVector2D BR);
private:
    void updateVertices(float the_size_x, float the_size_y);

private:
    float size_x;
    float size_y;
    Camera * m_camera;
    Texture * m_texture;
    VertexData vertices[4];
    GLushort indices[6];
    GLuint _vbo[2];
    ShaderProgram * m_shader;
};

#endif // SPRITE_H
