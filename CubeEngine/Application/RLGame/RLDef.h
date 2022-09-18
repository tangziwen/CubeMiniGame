#pragma once

#define ARENA_MAP_SIZE 18
#define AREAN_COLLISION_MAP_PADDING 32

enum RL_OBJECT_TYPE
{
	RL_OBJECT_TYPE_HERO,
	RL_OBJECT_TYPE_MONSTER,
	RL_OBJECT_TYPE_BULLET
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