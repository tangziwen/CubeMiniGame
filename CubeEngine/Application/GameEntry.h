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
	void onStart() override;
	void onExit() override;
	void onUpdate(float delta) override;
private:
	float m_ticks;
};


class TestVulkanEntry :public tzw::AppEntry
{
public:
	TestVulkanEntry();
	void onStart() override;
	void onExit() override;
	void onUpdate(float delta) override;
private:
	float m_ticks;
};
#endif // MYAPPDELEGATE_H
