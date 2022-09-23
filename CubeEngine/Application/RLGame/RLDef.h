#pragma once

#define ARENA_MAP_SIZE 18
#define AREAN_COLLISION_MAP_PADDING 32

enum RL_OBJECT_TYPE
{
	RL_OBJECT_TYPE_HERO,
	RL_OBJECT_TYPE_MONSTER,
	RL_OBJECT_TYPE_BULLET
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