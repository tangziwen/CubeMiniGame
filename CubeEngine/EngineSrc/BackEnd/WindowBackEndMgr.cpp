#include "WindowBackEndMgr.h"
#include "GLFW/GLFW_BackEnd.h"
namespace tzw {
TZW_SINGLETON_IMPL(WindowBackEndMgr)

WindowBackEnd *WindowBackEndMgr::getWindowBackEnd(int type)
{
    switch(type)
    {
    case TZW_WINDOW_GLFW:
    {
        return GLFW_BackEnd::shared();
    }
        break;
    default:
        return nullptr;
    }
}

} // namespace tzw
