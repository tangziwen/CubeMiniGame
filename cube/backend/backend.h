#ifndef BACKEND_H
#define BACKEND_H
#include "base/renderdelegate.h"
class backend
{
public:
   static int start(RenderDelegate * delegate, int argc, char *argv[]);
};

#endif // BACKEND_H
