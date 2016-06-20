#include "Event.h"
#include "EventMgr.h"
namespace tzw {

bool EventListener::onKeyPress(std::string keyCode)
{
    return false;
}

EventListener::~EventListener()
{
    EventMgr::shared()->removeEventListener(this);
}

bool EventListener::onKeyRelease(std::string keyCode)
{
    return false;
}

bool EventListener::onMouseRelease(int button, vec2 pos)
{
    return false;
}

bool EventListener::onMousePress(int button, vec2 pos)
{
    return false;
}

bool EventListener::onMouseMove(vec2 pos)
{
    return false;
}

void EventListener::onFrameUpdate(float delta)
{

}

EventListener::EventListener()
    :m_fixedPiority(0),m_isSwallow(false)
{

}

unsigned int EventListener::getFixedPiority() const
{
    return m_fixedPiority;
}

void EventListener::setFixedPiority(unsigned int eventPiority)
{
    m_fixedPiority = eventPiority;
    if (m_parent)
    {
        m_parent->sortFixedListener();
    }
}

bool EventListener::isSwallow() const
{
    return m_isSwallow;
}

void EventListener::setIsSwallow(bool isSwallow)
{
    m_isSwallow = isSwallow;
}

EventMgr *EventListener::parent() const
{
    return m_parent;
}

void EventListener::setParent(EventMgr *parent)
{
    m_parent = parent;
}

Node *EventListener::attachNode() const
{
    return m_attachNode;
}

void EventListener::setAttachNode(Node *attachNode)
{
    m_attachNode = attachNode;
}

} // namespace tzw

