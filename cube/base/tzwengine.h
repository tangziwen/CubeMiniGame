#ifndef TZWENGINE_H
#define TZWENGINE_H
#include "renderdelegate.h"
class TzwEngine
{
public:
    static int go(RenderDelegate * delegate, int argc, char *argv[]);
    static RenderDelegate * delegate;
};

#endif // TZWENGINE_H
