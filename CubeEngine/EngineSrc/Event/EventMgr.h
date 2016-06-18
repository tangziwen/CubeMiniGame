#ifndef TZW_EVENTMGR_H
#define TZW_EVENTMGR_H
#include <vector>
#include <deque>
#include <string>
#include "../EngineSrc/Math/vec2.h"
#include "../Engine/EngineDef.h"
namespace tzw {
struct EventInfo
{
    int type;
    std::string keycode;
    int arg;
    vec2 pos;
};
class Event;
class EventMgr
{
public:
    TZW_SINGLETON_DECL(EventMgr)
    void addEventListener(Event * event);
    void handleKeyPress(std::string keyCode);
    void handleKeyRelease(std::string keyCode);
    void handleMousePress(int button,vec2 pos);
    void handleMouseRelease(int button,vec2 pos);
    void handleMouseMove(vec2 pos);
    void apply(float delta);
    void removeEventListener(Event * event);
    void sortEvents();
private:
    EventMgr();
    std::vector<Event *>m_list;
    std::deque<EventInfo>m_eventDeque;
};

} // namespace tzw

#endif // TZW_EVENTMGR_H
