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
class ParticleEmitter;
class ProjectileBullet : public Bullet
{
public:
	ProjectileBullet(PhysicsRigidBody * rigidBody);
	virtual ~ProjectileBullet();
private:
	void onHitCallBack(vec3 p);
	PhysicsRigidBody * m_rigidBody;
	ParticleEmitter * m_hitSfx;
};
}
