#include "Event.h"
#include "EventMgr.h"
namespace tzw {

bool EventListener::onKeyPress(int keyCode)
{
    return false;
}

EventListener::~EventListener()
{
    EventMgr::shared()->removeEventListener(this);
}

bool EventListener::onKeyRelease(int keyCode)
{
    return false;
}

bool EventListener::onCharInput(unsigned int theChar)
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

bool EventListener::onScroll(vec2 offset)
{
	return false;
}

void EventListener::onFrameUpdate(float delta)
{

}

EventListener::EventListener()
		: m_attachNode(nullptr), m_mgr(nullptr), m_isSwallow(true), m_fixedPiority(0)
{

}

unsigned int EventListener::getFixedPiority() const
{
    return m_fixedPiority;
}

void EventListener::setFixedPiority(unsigned int eventPiority)
{
    m_fixedPiority = eventPiority;
    if (m_mgr)
    {
        m_mgr->sortFixedListener();
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

EventMgr *EventListener::getMgr() const
{
    return m_mgr;
}

void EventListener::setMgr(EventMgr *parent)
{
    m_mgr = parent;
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

