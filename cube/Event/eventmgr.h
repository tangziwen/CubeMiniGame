#ifndef EVENTMGR_H
#define EVENTMGR_H

#include <vector>
#include "listener/touchable.h"
#include <QVector2D>
class EventMgr
{
public:
    static EventMgr *get();
    void addTouchableListener(Touchable * listener);
    void raiseOnTouchPress(QVector2D pos);
    void raiseOnTouchRelease(QVector2D pos);
private:
    EventMgr();
    static EventMgr * instance;
private:

    //touchable
    std::vector<Touchable *> touchableList;
    Touchable * m_currentHoldTouchableListener;
};

#endif // EVENTMGR_H
