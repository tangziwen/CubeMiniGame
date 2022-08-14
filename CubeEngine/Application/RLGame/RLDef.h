#pragma once

#define ARENA_MAP_SIZE 32
#define AREAN_COLLISION_MAP_PADDING 32

enum RL_OBJECT_TYPE
{
	RL_OBJECT_TYPE_HERO,
	RL_OBJECT_TYPE_MONSTER
};

enum class RL_GameState
{
	MainMenu,
	Prepare,
	Playing,
	AfterMath,
	Win,
	Purchase
};