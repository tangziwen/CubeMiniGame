#ifndef MYAPPDELEGATE_H
#define MYAPPDELEGATE_H
#include "EngineSrc/Engine/AppDelegate.h"

class MyAppDelegate :public tzw::AppDelegate
{
public:
    MyAppDelegate();
    virtual void onStart();
    virtual void onExit();
    virtual void onUpdate(float delta);

};

#endif // MYAPPDELEGATE_H
