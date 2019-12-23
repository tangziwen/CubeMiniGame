#include "Bullet.h"
#include "Scene/SceneMgr.h"
#include "BuildingSystem.h"
#include "2D/LabelNew.h"
#include "Collision/PhysicsMgr.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Primitive/CubePrimitive.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "3D/Primitive/LaserPrimitive.h"

namespace tzw
{
	Bullet::Bullet():m_currDuration(0.0),m_duration(1.0)
	{
	}

	Bullet::~Bullet()
	{
		
	}

	float Bullet::getDuration() const
	{
		return m_duration;
	}

	void Bullet::setDuration(const float duration)
	{
		m_duration = duration;
	}

	float Bullet::getCurrDuration() const
	{
		return m_currDuration;
	}

	void Bullet::setCurrDuration(const float currDuration)
	{
		m_currDuration = currDuration;
	}

	void Bullet::update(float dt)
	{
	}

	void Bullet::setIsFirstTimeHit(const bool isFirstTimeHit)
	{
		m_isFirstTimeHit = isFirstTimeHit;
	}

	bool Bullet::isIsFirstTimeHit() const
	{
		return m_isFirstTimeHit;
	}

}

