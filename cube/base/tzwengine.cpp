#include "tzwengine.h"
#include "stdlib.h"
#include "backend/backend.h"

#include "external/TUtility/TUtility.h"
RenderDelegate * TzwEngine::delegate=NULL;
#include <QDebug>
void writeFunc(const char * str)
{
    qDebug()<<str;
}

int TzwEngine::go(RenderDelegate *delegate,int argc, char *argv[])
{
    TzwEngine::delegate = delegate;
    tzw::TlogSystem::get ()->setWriteFunc (writeFunc);
    return backend::start(delegate,argc,argv);
}
