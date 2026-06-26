#include "GUIWindowMgr.h"
#include "GUITitledFrame.h"
#include "../Engine/EngineDef.h"
#include "../Scene/SceneMgr.h"
#include <algorithm>
namespace tzw {
void GUIWindowMgr::focus(GUIWindow *window)
{
    auto result = std::find(frameList.begin(),frameList.end(),window);
    window->setLocalPriority(frameList.size());
	if(result != frameList.end())
		frameList.erase(result);
    frameList.push_back(window);
    for (size_t i = 0; i<frameList.size();i++)
    {
        auto wnd = frameList[i];
        wnd->setLocalPriority(i);
    }
}

void GUIWindowMgr::add(GUIWindow *frame)
{
    frameList.push_back(frame);
    frame->setLocalPriority(frameList.size());
    g_GetCurrScene()->addNode(frame);
}

GUIWindowMgr::GUIWindowMgr()
{
}

} // namespace tzw
