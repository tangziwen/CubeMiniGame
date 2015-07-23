#include "EventMgr.h"
#include "listener/TouchListener.h"
#include "listener/KeyListener.h"
#include "listener/RenderListener.h"
EventMgr * EventMgr::instance = nullptr;

EventMgr *EventMgr::get()
{
    if(instance)
    {
        return instance;
    }else{
        instance = new EventMgr();
        return instance;
    }
}

void EventMgr::addListener(BaseListener *listener)
{
    switch(listener->getListenerType ())
    {
    case BaseListener::Type::KeyListener:
        m_keyListeners.push_back (static_cast<KeyListener*>(listener));
        break;
    case BaseListener::Type::TouchListener:
        m_touchListeners.push_back (static_cast<TouchListener*>(listener));
        break;
    case BaseListener::Type::RenderListener:
        m_renderListeners.push_back (static_cast<RenderListener*>(listener));
        break;
    default:
        break;
    }
}

void EventMgr::raiseOnTouchPress(QVector2D pos)
{
    for(int i =0; i<m_touchListeners.size (); i++)
    {
        TouchListener * listener = m_touchListeners.at (i);
            listener->handleTouchPress (pos);

            auto callBack = listener->onTouchPress;
            if(callBack) callBack(listener,pos);
            break;
    }
}

void EventMgr::raiseOnTouchMove(QVector2D pos)
{
    for(int i =0; i<m_touchListeners.size (); i++)
    {
        TouchListener * listener = m_touchListeners.at (i);
            listener->handleTouchMove (pos);

            auto callBack = listener->onTouchMove;
            if(callBack) callBack(listener,pos);
            break;
    }
}

void EventMgr::raiseOnTouchRelease(QVector2D pos)
{
    for(int i =0; i<m_touchListeners.size (); i++)
    {
        TouchListener * listener = m_touchListeners.at (i);
        listener->handleTouchRelease (pos);
        auto callBack = listener->onTouchRelease;
        if(callBack) callBack(listener,pos);
    }
}

void EventMgr::raiseOnKeyPress(int keyCode)
{
    for(KeyListener * listener : m_keyListeners)
    {
        listener->handleKeyPress (keyCode);
        auto callBack = listener->onKeyPress;
        if(callBack) callBack(listener,keyCode);
    }
}

void EventMgr::raiseOnKeyRelease(int keyCode)
{
    for(KeyListener * listener : m_keyListeners)
    {
        listener->handleKeyRelease (keyCode);
        auto callBack = listener->onKeyRelease;
        if(callBack) callBack(listener,keyCode);
    }
}

void EventMgr::raiseOnRender(float dt)
{
    for(RenderListener * listener : m_renderListeners)
    {
        listener->handleOnRender (dt);
        auto callBack = listener->onRender;
        if(callBack) callBack(listener,dt);
    }
}

EventMgr::EventMgr()
{

}

