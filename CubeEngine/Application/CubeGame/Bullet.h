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
class ParticleEmitter;
class Bullet
{
public:
	Bullet(PhysicsRigidBody * rigidBody);
	~Bullet();
private:
	void onHitCallBack(vec3 p);
	bool m_isFirstTimeHit;
	PhysicsRigidBody * m_rigidBody;
	float m_duration;
	ParticleEmitter * m_hitSfx;
public:
	float getDuration() const;
	void setDuration(const float duration);
	float getCurrDuration() const;
	void setCurrDuration(const float currDuration);
private:
	float m_currDuration;
public:
	void setIsFirstTimeHit(const bool isFirstTimeHit);
	bool isIsFirstTimeHit() const;
};
}
