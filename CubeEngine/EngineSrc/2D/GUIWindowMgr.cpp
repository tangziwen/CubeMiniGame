#include "GUIWindowMgr.h"
#include "GUITitledFrame.h"
#include "../Engine/EngineDef.h"
#include "../Scene/SceneMgr.h"
namespace tzw {
TZW_SINGLETON_IMPL(GUIWindowMgr)
void GUIWindowMgr::focus(GUIWindow *window)
{
    auto result = std::find(frameList.begin(),frameList.end(),window);
    window->setLocalPiority(frameList.size());
    frameList.erase(result);
    frameList.push_back(window);
    for (size_t i = 0; i<frameList.size();i++)
    {
        auto wnd = frameList[i];
        wnd->setLocalPiority(i);
    }
}

void GUIWindowMgr::add(GUIWindow *frame)
{
    frameList.push_back(frame);
    frame->setLocalPiority(frameList.size());
    SceneMgr::shared()->currentScene()->addNode(frame);
}

GUIWindowMgr::GUIWindowMgr()
{
}

} // namespace tzw
