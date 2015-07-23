#ifndef SPRITE_H
#define SPRITE_H
#include "shader/ShaderPool.h"
#include "texture/texturepool.h"
#include "base/node.h"
#include "base/vertexdata.h"
#include "base/camera.h"
#include "listener/TouchListener.h"

#include <vector>
#include <QGLFunctions>
#include <functional>

//Sprite class , use to handle GUI element
class Sprite : public Node, public QGLFunctions, public TouchListener
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
    void setRect(QVector2D TL, QVector2D BR);
    QVector2D getSize();
    void setSize(int width,int height);
    QVector2D TL() const;
    void setTL(const QVector2D &TL);

    QVector2D BR() const;
    void setBR(const QVector2D &BR);

private:
    QVector2D m_TL;
    QVector2D m_BR;
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
