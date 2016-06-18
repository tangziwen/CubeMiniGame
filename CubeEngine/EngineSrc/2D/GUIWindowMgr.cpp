#include "GUIWindowMgr.h"
#include "GUITitledFrame.h"
#include "../Engine/EngineDef.h"
#include "../Scene/SceneMgr.h"
namespace tzw {
TZW_SINGLETON_IMPL(GUIWindowMgr)
void GUIWindowMgr::focus(GUITitledFrame *window)
{
    auto result = std::find(frameList.begin(),frameList.end(),window);
    window->setPiority(frameList.size());
    frameList.erase(result);
    for (auto wnd :frameList)
    {
        wnd->setPiority(wnd->getPiority() - 1);
        window->setEventPiority(wnd->getPiority());
    }
    frameList.push_back(window);
}

void GUIWindowMgr::add(GUITitledFrame *frame)
{
    frameList.push_back(frame);
    frame->setPiority(frameList.size());
    SceneMgr::shared()->currentScene()->addNode(frame);
}

GUIWindowMgr::GUIWindowMgr()
{
}

} // namespace tzw
