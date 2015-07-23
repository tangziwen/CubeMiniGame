#ifndef CLIKABLE_H
#define CLIKABLE_H
#include <functional>
#include "BaseListener.h"
class KeyListener : public BaseListener
{
public:
    KeyListener();
    ~KeyListener();
    virtual void handleKeyPress(int keycode);
    virtual void handleKeyRelease(int keycode);
    std::function<void (KeyListener * self, int keyCode)> onKeyPress;
    std::function<void (KeyListener * self, int keyCode)> onKeyRelease;

    virtual Type getListenerType() ;
};

#endif // CLIKABLE_H
