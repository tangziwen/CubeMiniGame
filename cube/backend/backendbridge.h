#ifndef BACKENDBRIDGE_H
#define BACKENDBRIDGE_H
#include <qopenglfunctions>
class BackendBridge : protected QOpenGLFunctions
{
public:
    BackendBridge();
};

#endif // BACKENDBRIDGE_H
