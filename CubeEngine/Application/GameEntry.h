#ifndef MYAPPDELEGATE_H
#define MYAPPDELEGATE_H
#include "EngineSrc/Engine/AppEntry.h"

class GameEntry :public tzw::AppEntry
{
public:
    GameEntry();
    virtual void onStart();
    virtual void onExit();
    virtual void onUpdate(float delta);
};

#endif // MYAPPDELEGATE_H
