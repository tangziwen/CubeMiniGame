#ifndef TZW_GUIWINDOWMGR_H
#define TZW_GUIWINDOWMGR_H

#include "../Engine/EngineDef.h"
#include <deque>
namespace tzw {
class GUITitledFrame;
class GUIWindowMgr
{
public:
    void focus(GUITitledFrame * window);
    void add(GUITitledFrame * frame);
private:
    std::deque<GUITitledFrame *> frameList;
    GUIWindowMgr();

    TZW_SINGLETON_DECL(GUIWindowMgr)
};

} // namespace tzw

#endif // TZW_GUIWINDOWMGR_H
