#ifndef BASELISTENER_H
#define BASELISTENER_H


class BaseListener
{
public:
    enum class Type
    {
        KeyListener,
        TouchListener,
        RenderListener,
    };

    BaseListener();
    virtual Type getListenerType() = 0;
};

#endif // BASELISTENER_H
