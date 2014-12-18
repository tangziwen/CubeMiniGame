#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H
#include <qopenglfunctions_3_0.h>

class RenderBuffer: protected QOpenGLFunctions_3_0
{
public:
    RenderBuffer();
    ~RenderBuffer();
};

#endif // RENDERBUFFER_H
