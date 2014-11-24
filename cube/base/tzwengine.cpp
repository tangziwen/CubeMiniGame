#include "tzwengine.h"
#include "stdlib.h"
#include "backend/backend.h"
RenderDelegate * TzwEngine::delegate=NULL;

int TzwEngine::go(RenderDelegate *delegate,int argc, char *argv[])
{
    TzwEngine::delegate = delegate;
    return backend::start(delegate,argc,argv);
}
