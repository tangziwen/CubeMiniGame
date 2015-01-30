#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H
#include <qopenglfunctions_3_0.h>

class RenderBuffer: protected QOpenGLFunctions_3_0
{
public:
    RenderBuffer(int width, int height);
    ~RenderBuffer();
    void BindForWriting();
    void BindForReading(RenderBuffer *buffer);
    GLuint buffer() const;
    void setBuffer(const GLuint &buffer);

private:
    GLuint m_buffer;
    GLuint m_texture;
};

#endif // RENDERBUFFER_H
