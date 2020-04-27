#include "WindowBackEndMgr.h"
#include "GLFW/GLFW_BackEnd.h"
namespace tzw {

WindowBackEnd *WindowBackEndMgr::getWindowBackEnd(int type)
{
	WindowBackEnd * backEnd = nullptr;
    switch(type)
    {
    case TZW_WINDOW_GLFW:
    {
        return backEnd = new GLFW_BackEnd();
    }
    default:
        return nullptr;
    }
	return backEnd;
}
} // namespace tzw
