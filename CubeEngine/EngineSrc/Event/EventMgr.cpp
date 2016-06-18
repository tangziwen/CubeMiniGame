#include "EventMgr.h"
#include "Event.h"
#include <algorithm>
#define EVENT_TYPE_K_RELEASE 0
#define EVENT_TYPE_K_PRESS 1
#define EVENT_TYPE_M_RELEASE 2
#define EVENT_TYPE_M_PRESS 3
#define EVENT_TYPE_M_MOVE 4
namespace tzw {

TZW_SINGLETON_IMPL(EventMgr)

void EventMgr::addEventListener(Event *event)
{
    m_list.push_back(event);
    event->setParent(this);
    sortEvents();
}

void EventMgr::handleKeyPress(std::string keyCode)
{
    EventInfo info;
    info.keycode = keyCode;
    info.type = EVENT_TYPE_K_PRESS;
    m_eventDeque.push_back(info);

}

void EventMgr::handleKeyRelease(std::string keyCode)
{
    EventInfo info;
    info.keycode = keyCode;
    info.type = EVENT_TYPE_K_RELEASE;
    m_eventDeque.push_back(info);

}

void EventMgr::handleMousePress(int button, vec2 pos)
{
    EventInfo info;
    info.pos = pos;
    info.arg = button;
    info.type = EVENT_TYPE_M_PRESS;
    m_eventDeque.push_back(info);

}

void EventMgr::handleMouseRelease(int button, vec2 pos)
{
    EventInfo info;
    info.pos = pos;
    info.arg = button;
    info.type = EVENT_TYPE_M_RELEASE;
    m_eventDeque.push_back(info);


}

void EventMgr::handleMouseMove(vec2 pos)
{
    EventInfo info;
    info.pos = pos;
    info.type = EVENT_TYPE_M_MOVE;
    m_eventDeque.push_back(info);


}

void EventMgr::apply(float delta)
{
    while(!m_eventDeque.empty())
    {
        EventInfo info = m_eventDeque.front();
        switch(info.type)
        {
        case EVENT_TYPE_K_PRESS:
        {
            for(auto i =0;i<m_list.size();i++)
            {
                Event * event = m_list[i];
                if(event->onKeyPress(info.keycode) && event->isSwallow())
                {
                    break;
                }
            }

        }
            break;
        case EVENT_TYPE_K_RELEASE:
        {
            for(auto i =0;i<m_list.size();i++)
            {
                Event * event = m_list[i];
                if(event->onKeyRelease(info.keycode)&&event->isSwallow())
                {
                    break;
                }
            }
        }
            break;
        case EVENT_TYPE_M_MOVE:
        {
            for(auto i =0;i<m_list.size();i++)
            {
                Event * event = m_list[i];
                if(event->onMouseMove(info.pos)&& event->isSwallow())
                {
                    break;
                }
            }
        }
            break;
        case EVENT_TYPE_M_PRESS:
        {
            for(auto i =0;i<m_list.size();i++)
            {
                Event * event = m_list[i];
                if (event->onMousePress(info.arg,info.pos) && event->isSwallow())
                {
                    break;
                }
            }
        }
            break;
        case EVENT_TYPE_M_RELEASE:
        {
            for(auto i =0;i<m_list.size();i++)
            {
                Event * event = m_list[i];
                if(event->onMouseRelease(info.arg,info.pos) && event->isSwallow())
                {
                    break;
                }
            }
        }
            break;
        }
        m_eventDeque.pop_front();
    }
    for(auto i =0;i<m_list.size();i++)
    {
        Event * event = m_list[i];
        event->onFrameUpdate(delta);
    }
}

void EventMgr::removeEventListener(Event *event)
{
    auto result = std::find(m_list.begin(),m_list.end(),event);
    if(result != m_list.end())
    {
        m_list.erase(result);
    }
}
static bool eventSort(const Event *a,const Event *b)
{
    return a->eventPiority() < b->eventPiority();
}

void EventMgr::sortEvents()
{
    std::stable_sort(m_list.rbegin(),m_list.rend(),eventSort);
}

EventMgr::EventMgr()
{

}

} // namespace tzw

