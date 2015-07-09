#ifndef CLIKABLE_H
#define CLIKABLE_H
#include <functional>

class Clickable
{
public:
    Clickable();
    ~Clickable();
    virtual void handleKeyPress(int keycode);
    virtual void handleKeyRelease(int keycode);
    std::function<void (Clickable * self, int keyCode)> onKeyPress;
    std::function<void (Clickable * self, int keyCode)> onKeyRelease;
};

#endif // CLIKABLE_H
