#ifndef MYAPPDELEGATE_H
#define MYAPPDELEGATE_H
#include "EngineSrc/Engine/AppEntry.h"

#define GAME_MODE_PLAYGROUND 1
#define GAME_MODE_SIM 2

#define CURRGAME GAME_MODE_PLAYGROUND

class GameEntry :public tzw::AppEntry
{
public:
	GameEntry();
	virtual void onStart();
	virtual void onExit();
	virtual void onUpdate(float delta);
private:
	float m_ticks;
};

#endif // MYAPPDELEGATE_H
