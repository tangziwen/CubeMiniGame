#pragma once

#define ARENA_MAP_SIZE 20
#define AREAN_COLLISION_MAP_PADDING 32
#define ARENA_MAP_TILE_SIZE 32
#include <functional>
class b2Body;
class b2Contact;
enum RL_OBJECT_TYPE
{
	RL_OBJECT_TYPE_HERO,
	RL_OBJECT_TYPE_MONSTER,
	RL_OBJECT_TYPE_BULLET
};
struct RLUserDataWrapper
{
	RLUserDataWrapper(void * userData, int tag):m_userData(userData), m_tag(tag){};
	RLUserDataWrapper():m_userData(nullptr), m_tag(0){};
	void * m_userData = nullptr;
	int m_tag = 0;
	std::function <void(b2Body * , b2Body *, b2Contact*)> m_cb;
};

enum RL_COLLIDER_FLAG {
	RL_OBSTACLE =          1<<1,
	RL_PLAYER =     1<<2,
	RL_ENEMY =        1<<3,
	RL_PLAYER_BULLET = 1<<4,
	RL_ENEMY_BULLET =    1<<5,
};
enum class RL_GameState
{
	MainMenu,
	Prepare,
	Playing,
	Pause,
	AfterMath,
	Win,
	Purchase
};