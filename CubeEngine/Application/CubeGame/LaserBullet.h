#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include <map>
#include "Bullet.h"

namespace tzw
{
class LaserPrimitive;
class ParticleEmitter;
class LaserBullet :public Bullet
{
public:
	LaserBullet(LaserPrimitive * laser);
	~LaserBullet();
private:
	LaserPrimitive * m_laser;
	ParticleEmitter * m_hitSfx;
};
}
