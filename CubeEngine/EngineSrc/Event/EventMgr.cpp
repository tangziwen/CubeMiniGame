#include "EventMgr.h"
#include "Event.h"
#include <algorithm>
#include "../Scene/SceneMgr.h"
#include "../Base/Node.h"
#include "ScriptPy/ScriptPyMgr.h"
#define EVENT_TYPE_K_RELEASE 0
#define EVENT_TYPE_K_PRESS 1
#define EVENT_TYPE_M_RELEASE 2
#define EVENT_TYPE_M_PRESS 3
#define EVENT_TYPE_M_MOVE 4
#define EVENT_TYPE_K_CHAR_INPUT 5
#define EVENT_TYPE_M_SCROLL 6
namespace tzw {

namespace
{
bool isKeyEvent(int eventType)
{
	return eventType == EVENT_TYPE_K_PRESS
		|| eventType == EVENT_TYPE_K_RELEASE
		|| eventType == EVENT_TYPE_K_CHAR_INPUT;
}

bool isPointerEvent(int eventType)
{
	return eventType == EVENT_TYPE_M_MOVE
		|| eventType == EVENT_TYPE_M_SCROLL
		|| eventType == EVENT_TYPE_M_PRESS
		|| eventType == EVENT_TYPE_M_RELEASE;
}
}

EventMgr::EventMgr()
	: m_isNeedSortNodeListener(true)
{
}

void EventMgr::addListener(EventListener *listener)
{
    addFixedPiorityListener(listener);
}

void EventMgr::addListener(EventListener *listener, Node *node)
{
    addNodePiorityListener(node,listener);
}

void EventMgr::addFixedPiorityListener(EventListener *event)
{
    m_list.push_back(event);
    event->setMgr(this);
    sortFixedListener();
}

void EventMgr::addNodePiorityListener(Node *node, EventListener *event)
{
    auto result = m_nodeListenerMap.find(node);
    if (result != m_nodeListenerMap.end()) return;
    m_nodeListenerMap[node] = event;
    event->setAttachNode(node);
	notifyListenerChange();
}

void EventMgr::setCaptureListener(EventListener* event)
{
	m_captureListener = event;
	if (m_captureListener)
	{
		m_captureListener->setMgr(this);
	}
}

static bool nodeEventCompare(const EventListener *a,const EventListener *b)
{
    auto NodeA = a->attachNode();
    auto NodeB = b->attachNode();
    return NodeA->getGlobalPiority() > NodeB->getGlobalPiority();
}

void EventMgr::sortNodePiorityListener()
{
    auto root = g_GetCurrScene()->root();
    m_NodePioritylist.clear();
    visitNode(root);
    std::stable_sort(m_NodePioritylist.begin(),m_NodePioritylist.end(),nodeEventCompare);
}

void EventMgr::notifyListenerChange()
{
	m_isNeedSortNodeListener = true;
}

bool EventMgr::isMouseButtonConsumed(int button) const
{
	return button >= 0 && button < 3 && m_mouseButtonConsumed[button];
}

void EventMgr::handleKeyPress(int keyCode)
{
    EventInfo info;
    info.keycode = keyCode;
    info.type = EVENT_TYPE_K_PRESS;
    m_eventDeque.push_back(info);
}

void EventMgr::handleKeyRelease(int keyCode)
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

void EventMgr::handleCharInput(unsigned int theChar)
{
    EventInfo info;
    info.theChar = theChar;
    info.type = EVENT_TYPE_K_CHAR_INPUT;
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

void EventMgr::handleScroll(vec2 offset)
{
    EventInfo info;
    info.offset = offset;
    info.type = EVENT_TYPE_M_SCROLL;
    m_eventDeque.push_back(info);
}

void EventMgr::apply(float delta)
{
	beginFrame();
	dispatchQueuedKeyEvents();
	dispatchQueuedPointerEvents();
	updateFrameListeners(delta);
}

void EventMgr::beginFrame()
{
	for (bool& consumed : m_mouseButtonConsumed)
	{
		consumed = false;
	}
}

void EventMgr::dispatchQueuedKeyEvents()
{
	if(m_isNeedSortNodeListener)
	{
		m_isNeedSortNodeListener = false;
		sortNodePiorityListener();
	}

	std::deque<EventInfo> pendingPointerEvents;
	while(!m_eventDeque.empty())
	{
		EventInfo info = m_eventDeque.front();
		m_eventDeque.pop_front();
		if (!isKeyEvent(info.type))
		{
			pendingPointerEvents.push_back(info);
			continue;
		}

		switch(info.type)
		{
		case EVENT_TYPE_K_PRESS:
		{
			applyKeyPress(info);
			ScriptPyMgr::shared()->raiseInputEvent(info);
		}
			break;
		case EVENT_TYPE_K_RELEASE:
		{
			applyKeyRelease(info);
			ScriptPyMgr::shared()->raiseInputEvent(info);
		}
			break;
		case EVENT_TYPE_K_CHAR_INPUT:
		{
			applyKeyCharInput(info);
		}
			break;
		default:
			break;
		}
	}
	m_eventDeque.swap(pendingPointerEvents);
}

void EventMgr::updateFrameListeners(float delta)
{
	for(size_t i =0;i<m_list.size();i++)
	{
		EventListener * event = m_list[i];
		event->onFrameUpdate(delta);
	}
}

void EventMgr::dispatchQueuedPointerEvents()
{
	bool hasPointerEvent = false;
	for (const EventInfo& info : m_eventDeque)
	{
		if (isPointerEvent(info.type))
		{
			hasPointerEvent = true;
			break;
		}
	}
	if (hasPointerEvent)
	{
		m_isNeedSortNodeListener = false;
		sortNodePiorityListener();
	}

	std::deque<EventInfo> pendingKeyEvents;
	while(!m_eventDeque.empty())
	{
		EventInfo info = m_eventDeque.front();
		m_eventDeque.pop_front();
		if (!isPointerEvent(info.type))
		{
			pendingKeyEvents.push_back(info);
			continue;
		}

		switch(info.type)
		{
		case EVENT_TYPE_M_MOVE:
		{
			applyMouseMove(info);
		}
			break;
		case EVENT_TYPE_M_SCROLL:
		{
			applyScroll(info);
       		ScriptPyMgr::shared()->raiseInputEvent(info);
		}
			break;
		case EVENT_TYPE_M_PRESS:
		{
			applyMousePress(info);
			ScriptPyMgr::shared()->raiseInputEvent(info);
		}
			break;
		case EVENT_TYPE_M_RELEASE:
		{
			applyMouseRelease(info);
			ScriptPyMgr::shared()->raiseInputEvent(info);
		}
			break;
		default:
			break;
		}
	}
	m_eventDeque.swap(pendingKeyEvents);
}

void EventMgr::visitNode(Node *node)
{
    if (!node->getIsVisible())
    {
        return;
    }
    if(m_nodeListenerMap.find(node) != m_nodeListenerMap.end())
    {
        m_NodePioritylist.push_front(m_nodeListenerMap[node]);
    }
    if(node->getChildrenAmount() > 0 )
    {
        auto amount = node->getChildrenAmount();
        for(size_t i = 0 ;i <amount;i++)
        {
            auto child = node->getChildByIndex(i);
            visitNode(child);
        }
    }
}

void EventMgr::applyKeyPress(EventInfo &info)
{
	if (dispatchCaptureKeyPress(info)) return;

    for (auto event :m_NodePioritylist)
    {
        if(event->onKeyPress(info.keycode) && event->isSwallow()) return;
    }
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if(event->onKeyPress(info.keycode) && event->isSwallow()) return;
    }
}

void EventMgr::applyKeyCharInput(EventInfo &info)
{
	if (dispatchCaptureKeyCharInput(info)) return;

    for (auto event :m_NodePioritylist)
    {
        if(event->onCharInput(info.theChar) && event->isSwallow()) return;
    }
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if(event->onCharInput(info.theChar) && event->isSwallow()) return;
    }
}

void EventMgr::applyKeyRelease(EventInfo &info)
{
	if (dispatchCaptureKeyRelease(info)) return;

    for (auto event :m_NodePioritylist)
    {
        if(event->onKeyRelease(info.keycode) && event->isSwallow()) return;
    }
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if(event->onKeyRelease(info.keycode)&&event->isSwallow()) return;
    }
}

void EventMgr::applyMousePress(EventInfo &info)
{
	if (dispatchCaptureMousePress(info))
	{
		markMouseButtonConsumed(info.arg);
		return;
	}

    for (auto event :m_NodePioritylist)
    {
        if(event->onMousePress(info.arg,info.pos) && event->isSwallow())
		{
			markMouseButtonConsumed(info.arg);
			return;
		}
    }
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if (event->onMousePress(info.arg,info.pos) && event->isSwallow())
		{
			markMouseButtonConsumed(info.arg);
			return;
		}
    }
}

void EventMgr::applyMouseRelease(EventInfo &info)
{
	if (dispatchCaptureMouseRelease(info))
	{
		markMouseButtonConsumed(info.arg);
		return;
	}

    for (auto event :m_NodePioritylist)
    {
        if(event->onMouseRelease(info.arg,info.pos) && event->isSwallow())
		{
			markMouseButtonConsumed(info.arg);
			return;
		}
    }
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if(event->onMouseRelease(info.arg,info.pos) && event->isSwallow())
		{
			markMouseButtonConsumed(info.arg);
			return;
		}
    }
}

void EventMgr::applyMouseMove(EventInfo &info)
{
	if (dispatchCaptureMouseMove(info)) return;

    for (auto event :m_NodePioritylist)
    {
        if(event->onMouseMove(info.pos)&& event->isSwallow()) return;
    }
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if(event->onMouseMove(info.pos)&& event->isSwallow()) return;
    }
}

void EventMgr::applyScroll(EventInfo& info)
{
	if (dispatchCaptureScroll(info)) return;

    for (auto event :m_NodePioritylist)
    {
        if(event->onScroll(info.offset)&& event->isSwallow()) return;
	}
    for(auto event : m_list)
    {
		if (event == m_captureListener) continue;
        if(event->onScroll(info.offset)&& event->isSwallow()) return;
    }
}

bool EventMgr::dispatchCaptureKeyPress(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onKeyPress(info.keycode)
		&& m_captureListener->isSwallow();
}

bool EventMgr::dispatchCaptureKeyRelease(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onKeyRelease(info.keycode)
		&& m_captureListener->isSwallow();
}

bool EventMgr::dispatchCaptureKeyCharInput(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onCharInput(info.theChar)
		&& m_captureListener->isSwallow();
}

bool EventMgr::dispatchCaptureMousePress(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onMousePress(info.arg, info.pos)
		&& m_captureListener->isSwallow();
}

bool EventMgr::dispatchCaptureMouseRelease(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onMouseRelease(info.arg, info.pos)
		&& m_captureListener->isSwallow();
}

bool EventMgr::dispatchCaptureMouseMove(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onMouseMove(info.pos)
		&& m_captureListener->isSwallow();
}

bool EventMgr::dispatchCaptureScroll(EventInfo& info)
{
	return m_captureListener
		&& m_captureListener->onScroll(info.offset)
		&& m_captureListener->isSwallow();
}

void EventMgr::markMouseButtonConsumed(int button)
{
	if (button >= 0 && button < 3)
	{
		m_mouseButtonConsumed[button] = true;
	}
}

EventMgr::~EventMgr()
{
	tlogError("on dispose");
}

void EventMgr::removeEventListener(EventListener *event)
{
	if (event == m_captureListener)
	{
		m_captureListener = nullptr;
	}
    auto result = std::find(m_list.begin(),m_list.end(),event);
    if(result != m_list.end())
    {
        m_list.erase(result);
    }
}

void EventMgr::removeNodeEventListener(Node* node)
{
    auto result = m_nodeListenerMap.find(node);
    if(result != m_nodeListenerMap.end())
    {
        m_nodeListenerMap.erase(result);
		notifyListenerChange();
    }
}

static bool eventSort(const EventListener *a,const EventListener *b)
{
    return a->getFixedPiority() > b->getFixedPiority();
}

void EventMgr::sortFixedListener()
{
    std::stable_sort(m_list.begin(),m_list.end(),eventSort);
}

} // namespace tzw

