#include "ProjectileBullet.h"
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
namespace tzw
{

	ProjectileBullet::ProjectileBullet(PhysicsRigidBody * rigidBody)
	:m_rigidBody(rigidBody),m_hitSfx(nullptr)
	{
		m_isFirstTimeHit = true;
		m_rigidBody->m_onHitCallBack = std::bind(&ProjectileBullet::onHitCallBack, this,std::placeholders::_1);
		m_duration = 0.0;
	}

	ProjectileBullet::~ProjectileBullet()
	{
		if(m_rigidBody)
		{
			tlog("remove");
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
	}

	void ProjectileBullet::update(float dt)
	{
		if(m_rigidBody)
		{
			tlog("bullet pos %s",m_rigidBody->parent()->getWorldPos().getStr().c_str());
		}
	}

	void ProjectileBullet::onHitCallBack(vec3 p)
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
}

