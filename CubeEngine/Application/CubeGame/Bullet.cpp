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

namespace tzw
{
	Bullet::Bullet(PhysicsRigidBody * rigidBody)
	:m_isFirstTimeHit(true),m_rigidBody(rigidBody), m_duration(1.5), m_currDuration(0.0),m_hitSfx(nullptr)
	{
		rigidBody->m_onHitCallBack = std::bind(&Bullet::onHitCallBack, this,std::placeholders::_1);
	}

	Bullet::~Bullet()
	{
		PhysicsMgr::shared()->removeRigidBody(m_rigidBody);
		auto node = m_rigidBody->parent();
		node->removeFromParent();
		delete m_rigidBody;
		delete node;
		if(m_hitSfx)
		{
			m_hitSfx->removeFromParent();
			delete m_hitSfx;
		}
	}

	void Bullet::onHitCallBack(vec3 p)
	{
		if(m_isFirstTimeHit)
		{
			auto emitter = new ParticleEmitter(1);
			emitter->setIsLocalPos(true);
			emitter->setTex("ParticleTex/smoke_04.png");
			emitter->setSpawnRate(0.3);
			emitter->addInitModule(new ParticleInitSizeModule(0.5, 0.7));
			//emitter->addInitModule(new ParticleInitVelocityModule(vec3(0, 0.0, 0.0), vec3(0, 0.0, 0.0 )));
			emitter->addInitModule(new ParticleInitLifeSpanModule(0.3, 0.3));
			emitter->addInitModule(new ParticleInitAlphaModule(0.6, 0.6));
			emitter->addUpdateModule(new ParticleUpdateColorModule(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.8, 0.8, 1.0, 0.0)));
	        emitter->addUpdateModule(new ParticleUpdateSizeModule(1.0, 1.5));
			emitter->setIsState(ParticleEmitter::State::Playing);
			emitter->setDepthBias(0.05);
			emitter->setPos(m_rigidBody->parent()->getWorldPos());
			emitter->setIsInfinite(false);
	        emitter->setBlendState(1);
	        g_GetCurrScene()->addNode(emitter);
			m_hitSfx = emitter;
			m_isFirstTimeHit = false;
		}
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

	void Bullet::setIsFirstTimeHit(const bool isFirstTimeHit)
	{
		m_isFirstTimeHit = isFirstTimeHit;
	}

	bool Bullet::isIsFirstTimeHit() const
	{
		return m_isFirstTimeHit;
	}

}

