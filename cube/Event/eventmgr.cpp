#include "eventmgr.h"
#include "listener/touchable.h"
#include "listener/Clickable.h"
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
            listener->handleTouchPress (pos);

            auto callBack = listener->onTouchPress;
            if(callBack) callBack(listener,pos);
            break;
    }
}

void EventMgr::raiseOnTouchRelease(QVector2D pos)
{
    for(int i =0; i<touchableList.size (); i++)
    {
        Touchable * listener = touchableList.at (i);
        listener->handleTouchRelease (pos);
        auto callBack = listener->onTouchRelease;
        if(callBack) callBack(listener,pos);
    }
}

void EventMgr::raiseOnKeyPress(int keyCode)
{
    for(Clickable * listener : clickableList)
    {
        listener->handleKeyPress (keyCode);
        auto callBack = listener->onKeyPress;
        if(callBack) callBack(listener,keyCode);
    }
}

void EventMgr::raiseOnKeyRelease(int keyCode)
{
    for(Clickable * listener : clickableList)
    {
        listener->handleKeyRelease (keyCode);
        auto callBack = listener->onKeyRelease;
        if(callBack) callBack(listener,keyCode);
    }
}

EventMgr::EventMgr()
{

}

