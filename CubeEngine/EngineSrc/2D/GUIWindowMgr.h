#ifndef TZW_GUIWINDOWMGR_H
#define TZW_GUIWINDOWMGR_H

#include "../Engine/EngineDef.h"
#include <deque>
namespace tzw {
class GUIWindow;
class GUIWindowMgr
{
public:
    void focus(GUIWindow * window);
    void add(GUIWindow * frame);
private:
    std::deque<GUIWindow *> frameList;
    GUIWindowMgr();

    TZW_SINGLETON_DECL(GUIWindowMgr)
};

} // namespace tzw

#endif // TZW_GUIWINDOWMGR_H
