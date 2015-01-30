#include "renderbuffer.h"

RenderBuffer::RenderBuffer(int width,int height)
{
    initializeOpenGLFunctions ();
    glGenFramebuffers (1,&m_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_buffer);
    glGenTextures (1,&m_texture);
    glBindTexture (GL_TEXTURE_2D,m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
}

RenderBuffer::~RenderBuffer()
{

}

void RenderBuffer::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_buffer);
}

void RenderBuffer::BindForReading(RenderBuffer* buffer)
{
    if(buffer)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer->buffer ());
    }else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D,m_texture);
}

GLuint RenderBuffer::buffer() const
{
    return m_buffer;
}

void RenderBuffer::setBuffer(const GLuint &buffer)
{
    m_buffer = buffer;
}



