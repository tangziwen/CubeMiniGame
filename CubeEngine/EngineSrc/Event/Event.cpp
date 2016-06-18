#include "Event.h"
#include "EventMgr.h"
namespace tzw {

bool Event::onKeyPress(std::string keyCode)
{
    return false;
}

Event::~Event()
{
    EventMgr::shared()->removeEventListener(this);
}

bool Event::onKeyRelease(std::string keyCode)
{
    return false;
}

bool Event::onMouseRelease(int button, vec2 pos)
{
    return false;
}

bool Event::onMousePress(int button, vec2 pos)
{
    return false;
}

bool Event::onMouseMove(vec2 pos)
{
    return false;
}

void Event::onFrameUpdate(float delta)
{

}

Event::Event()
    :m_eventPiority(0),m_isSwallow(false)
{

}

unsigned int Event::eventPiority() const
{
    return m_eventPiority;
}

void Event::setEventPiority(unsigned int eventPiority)
{
    m_eventPiority = eventPiority;
    if (m_parent)
    {
        m_parent->sortEvents();
    }
}

bool Event::isSwallow() const
{
    return m_isSwallow;
}

void Event::setIsSwallow(bool isSwallow)
{
    m_isSwallow = isSwallow;
}

EventMgr *Event::parent() const
{
    return m_parent;
}

void Event::setParent(EventMgr *parent)
{
    m_parent = parent;
}

} // namespace tzw

