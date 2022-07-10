#pragma once
#include "Math/vec2.h"

namespace tzw
{
class Sprite;
class SpriteInstanceInfo;
struct RLBullet
{
	vec2 m_pos;
	vec2 m_velocity;
	int m_typeID = 0;
	SpriteInstanceInfo * m_sprite = nullptr;
};
}
