#ifndef EVENTMGR_H
#define EVENTMGR_H

#include <vector>
class BaseListener;
class TouchListener;
class KeyListener;
class RenderListener;
#include <QVector2D>
class EventMgr
{
public:
    static EventMgr *get();
    void addListener(BaseListener * listener);
    void raiseOnTouchPress(QVector2D pos);
    void raiseOnTouchMove(QVector2D pos);
    void raiseOnTouchRelease(QVector2D pos);
    void raiseOnKeyPress(int keyCode);
    void raiseOnKeyRelease(int keyCode);
    void raiseOnRender(float dt);
private:
    EventMgr();
    static EventMgr * instance;
private:

    //touchable
    std::vector<TouchListener *> m_touchListeners;
    std::vector<KeyListener * > m_keyListeners;
    std::vector<RenderListener * > m_renderListeners;
};

#endif // EVENTMGR_H
