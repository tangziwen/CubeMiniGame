#include "eventmgr.h"

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

void EventMgr::addTouchableListener(Touchable *listener)
{
    touchableList.push_back (listener);
}

void EventMgr::raiseOnTouchPress(QVector2D pos)
{
    for(int i =0; i<touchableList.size (); i++)
    {
        Touchable * listener = touchableList.at (i);
        auto result = listener->checkTouchPress (pos);
        if(result){
            m_currentHoldTouchableListener = listener;
            listener->handleTouchPress (pos);
            break;
        }
    }
}

void EventMgr::raiseOnTouchRelease(QVector2D pos)
{
    if(m_currentHoldTouchableListener)
    {
        m_currentHoldTouchableListener->handleTouchRelease (pos);
        m_currentHoldTouchableListener = nullptr;
    }
}

EventMgr::EventMgr():
    m_currentHoldTouchableListener(nullptr)
{

}

