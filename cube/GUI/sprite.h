#ifndef SPRITE_H
#define SPRITE_H
#include "shader/shaderpoll.h"
#include "texture/texturepool.h"
#include "base/node.h"
#include "base/vertexdata.h"
#include "base/camera.h"
#include "listener/touchable.h"

#include <vector>
#include <QGLFunctions>
#include <functional>

//Sprite class , use to handle GUI element
class Sprite : public Node, public QGLFunctions, public Touchable
{
public:
    Sprite();
    Texture *texture() const;
    void setTexture(Texture *texture);
    void draw();
    ShaderProgram *shader() const;
    void setShader(ShaderProgram *shader);
    std::function<void (Sprite * self,float dt)> onRender;
    Camera *camera() const;
    void setCamera(Camera *camera);
    void setRect(QVector2D TL, QVector2D BR, float the_size_x, float the_size_y);
    QVector2D getSize();
private:
    float m_sizeX;
    float m_sizeY;
    Camera * m_camera;
    Texture * m_texture;
    VertexData m_vertices[4];
    GLushort m_indices[6];
    GLuint _vbo[2];
    ShaderProgram * m_shader;
};

#endif // SPRITE_H
