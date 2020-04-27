#ifndef TZW_GUIWINDOWMGR_H
#define TZW_GUIWINDOWMGR_H

#include "../Engine/EngineDef.h"
#include <deque>
namespace tzw {
class GUIWindow;
class GUIWindowMgr:public Singleton<GUIWindowMgr>
{
public:
    void focus(GUIWindow * window);
    void add(GUIWindow * frame);
	GUIWindowMgr();
private:
    std::deque<GUIWindow *> frameList;
    
};

} // namespace tzw

#endif // TZW_GUIWINDOWMGR_H
