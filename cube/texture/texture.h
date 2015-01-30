#ifndef TEXTURE_H
#define TEXTURE_H
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
class Texture : protected QOpenGLFunctions
{
public:
    Texture(const char * texture_file_name);
    int getTextureID();
    void setTextureID(unsigned int new_id);
    int height() const;
    void setHeight(int height);

    int width() const;
    void setWidth(int width);

private:
    QOpenGLTexture * m_qtexture;
    int m_height;
    int m_width;
    void initTexture();
    unsigned int texture_id;
};

#endif // TEXTURE_H
