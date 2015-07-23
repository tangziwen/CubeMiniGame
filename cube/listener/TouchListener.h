#ifndef TOUCHABLE_H
#define TOUCHABLE_H

#include <QVector2D>
#include <functional>
#include "BaseListener.h"
class TouchListener : public BaseListener
{
public:
    TouchListener();

    virtual void handleTouchPress(QVector2D pos);
    virtual void handleTouchMove(QVector2D pos);
    virtual void handleTouchRelease(QVector2D pos);

    std::function<void (TouchListener * self,QVector2D pos)> onTouchPress;
    std::function<void (TouchListener * self,QVector2D pos)> onTouchMove;
    std::function<void (TouchListener * self,QVector2D pos)> onTouchRelease;

    virtual Type getListenerType();
};

#endif // TOUCHABLE_H
