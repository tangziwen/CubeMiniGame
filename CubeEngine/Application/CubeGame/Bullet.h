#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include <map>
namespace tzw
{
	class LaserPrimitive;
	class ParticleEmitter;
class Bullet
{
public:
	Bullet();
	virtual ~Bullet();
	float getDuration() const;
	void setDuration(const float duration);
	float getCurrDuration() const;
	void setCurrDuration(const float currDuration);
	virtual void update(float dt);
	void setIsFirstTimeHit(const bool isFirstTimeHit);
	bool isIsFirstTimeHit() const;
protected:
	bool m_isFirstTimeHit;
	PhysicsRigidBody * m_rigidBody;
	LaserPrimitive * m_laser;
	float m_duration;
	ParticleEmitter * m_hitSfx;
	float m_currDuration;
};
}
