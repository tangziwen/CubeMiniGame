#ifndef TZW_WINDOWBACKEND_H
#define TZW_WINDOWBACKEND_H

#include "../Engine/EngineDef.h"
namespace tzw {
class WindowBackEnd
{
public:
    WindowBackEnd();
    virtual void prepare();
    virtual void run();
private:
    TZW_SINGLETON_DECL(WindowBackEnd)
};

} // namespace tzw

#endif // TZW_WINDOWBACKEND_H
