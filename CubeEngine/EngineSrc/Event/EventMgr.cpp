#include "EventMgr.h"
#include "Event.h"
#include <algorithm>
#include "../Scene/SceneMgr.h"
#include "../Base/Node.h"

#define EVENT_TYPE_K_RELEASE 0
#define EVENT_TYPE_K_PRESS 1
#define EVENT_TYPE_M_RELEASE 2
#define EVENT_TYPE_M_PRESS 3
#define EVENT_TYPE_M_MOVE 4
#define EVENT_TYPE_K_CHAR_INPUT 5
namespace tzw {

TZW_SINGLETON_IMPL(EventMgr)

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
    event->setParent(this);
    sortFixedListener();
}

void EventMgr::addNodePiorityListener(Node *node, EventListener *event)
{
    auto result = m_nodeListenerMap.find(node);
    if (result != m_nodeListenerMap.end()) return;
    m_nodeListenerMap[node] = event;
    event->setAttachNode(node);
}

static bool nodeEventCompare(const EventListener *a,const EventListener *b)
{
    auto NodeA = a->attachNode();
    auto NodeB = b->attachNode();
    return NodeA->getGlobalPiority() > NodeB->getGlobalPiority();
}

void EventMgr::sortNodePiorityListener()
{

    auto root = SceneMgr::shared()->currentScene()->root();
    m_NodePioritylist.clear();
    visitNode(root);
    std::stable_sort(m_NodePioritylist.begin(),m_NodePioritylist.end(),nodeEventCompare);
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

void EventMgr::apply(float delta)
{
    sortNodePiorityListener();
    while(!m_eventDeque.empty())
    {
        EventInfo info = m_eventDeque.front();
        switch(info.type)
        {
        case EVENT_TYPE_K_PRESS:
        {
            applyKeyPress(info);
        }
            break;
        case EVENT_TYPE_K_RELEASE:
        {
            applyKeyRelease(info);
        }
            break;
        case EVENT_TYPE_M_MOVE:
        {
            applyMouseMove(info);
        }
            break;
        case EVENT_TYPE_M_PRESS:
        {
            applyMousePress(info);
        }
            break;
        case EVENT_TYPE_M_RELEASE:
        {
            applyMouseRelease(info);
        }
            break;
        case EVENT_TYPE_K_CHAR_INPUT:
        {
            applyKeyCharInput(info);
        }
        }
        m_eventDeque.pop_front();
    }
    for(size_t i =0;i<m_list.size();i++)
    {
        EventListener * event = m_list[i];
        event->onFrameUpdate(delta);
    }
}

void EventMgr::visitNode(Node *node)
{
    if (!node->getIsDrawable())
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
    for(auto event : m_list)
    {
        if(event->onKeyPress(info.keycode) && event->isSwallow()) return;
    }
    for (auto event :m_NodePioritylist)
    {
        if(event->onKeyPress(info.keycode) && event->isSwallow()) return;
    }
}

void EventMgr::applyKeyCharInput(EventInfo &info)
{
    for(auto event : m_list)
    {
        if(event->onCharInput(info.theChar) && event->isSwallow()) return;
    }
    for (auto event :m_NodePioritylist)
    {
        if(event->onCharInput(info.theChar) && event->isSwallow()) return;
    }
}

void EventMgr::applyKeyRelease(EventInfo &info)
{
    for(auto event : m_list)
    {
        if(event->onKeyRelease(info.keycode)&&event->isSwallow()) return;
    }
    for (auto event :m_NodePioritylist)
    {
        if(event->onKeyRelease(info.keycode) && event->isSwallow()) return;
    }
}

void EventMgr::applyMousePress(EventInfo &info)
{
    for(auto event : m_list)
    {
        if (event->onMousePress(info.arg,info.pos) && event->isSwallow()) return;
    }
    for (auto event :m_NodePioritylist)
    {
        if(event->onMousePress(info.arg,info.pos) && event->isSwallow()) return;
    }
}

void EventMgr::applyMouseRelease(EventInfo &info)
{
    for(auto event : m_list)
    {
        if(event->onMouseRelease(info.arg,info.pos) && event->isSwallow()) return;
    }
    for (auto event :m_NodePioritylist)
    {
        if(event->onMouseRelease(info.arg,info.pos) && event->isSwallow()) return;
    }
}

void EventMgr::applyMouseMove(EventInfo &info)
{
    for(auto event : m_list)
    {
        if(event->onMouseMove(info.pos)&& event->isSwallow()) return;
    }
    for (auto event :m_NodePioritylist)
    {
        if(event->onMouseMove(info.pos)&& event->isSwallow()) return;
    }
}

void EventMgr::removeEventListener(EventListener *event)
{
    auto result = std::find(m_list.begin(),m_list.end(),event);
    if(result != m_list.end())
    {
        m_list.erase(result);
    }
}

static bool eventSort(const EventListener *a,const EventListener *b)
{
    return a->getFixedPiority() >= b->getFixedPiority();
}

void EventMgr::sortFixedListener()
{
    std::stable_sort(m_list.begin(),m_list.end(),eventSort);
}

} // namespace tzw

