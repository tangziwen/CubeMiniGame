#ifndef TZW_WINDOWBACKENDMGR_H
#define TZW_WINDOWBACKENDMGR_H
#include "../Engine/EngineDef.h"
#include "WindowBackEnd.h"
namespace tzw {
#define TZW_WINDOW_GLFW 0
#define TZW_WINDOW_SDL 1
#define TZW_WINDOW_QT 2
class WindowBackEndMgr
{
public:
    WindowBackEnd * getWindowBackEnd(int type);
    TZW_SINGLETON_DECL(WindowBackEndMgr)
};

} // namespace tzw

#endif // TZW_WINDOWBACKENDMGR_H
