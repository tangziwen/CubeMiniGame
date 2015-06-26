#ifndef EVENTMGR_H
#define EVENTMGR_H

#include <vector>
class Touchable;
class Clickable;

#include <QVector2D>
class EventMgr
{
public:
    static EventMgr *get();
    void addTouchableListener(Touchable * listener);
    void raiseOnTouchPress(QVector2D pos);
    void raiseOnTouchRelease(QVector2D pos);
    void raiseOnKeyPress(int keyCode);
    void raiseOnKeyRelease(int keyCode);
private:
    EventMgr();
    static EventMgr * instance;
private:

    //touchable
    std::vector<Touchable *> touchableList;
    std::vector<Clickable * > clickableList;
};

#endif // EVENTMGR_H
