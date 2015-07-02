#ifndef TOUCHABLE_H
#define TOUCHABLE_H

#include <QVector2D>
#include <functional>
class Touchable
{
public:
    Touchable();

    virtual void handleTouchPress(QVector2D pos);
    virtual void handleTouchRelease(QVector2D pos);

    std::function<void (Touchable * self,QVector2D pos)> onTouchPress;
    std::function<void (Touchable * self,QVector2D pos)> onTouchRelease;
};

#endif // TOUCHABLE_H
