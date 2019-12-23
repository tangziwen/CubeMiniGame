#include "BulletMgr.h"
#include "2D/LabelNew.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/LaserPrimitive.h"
#include "3D/Primitive/SpherePrimitive.h"
#include "BuildingSystem.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Collision/PhysicsMgr.h"
#include "LaserBullet.h"
#include "ProjectileBullet.h"
#include "Scene/SceneMgr.h"

namespace tzw {
TZW_SINGLETON_IMPL(BulletMgr);
BulletMgr::BulletMgr()
{
  m_isShowAssistInfo = false;
}

void
BulletMgr::handleDraw(float dt)
{
  auto i = m_bullets.begin();
  for (; i != m_bullets.end();) {
    auto b = (*i);
    b->update(dt);
    if (!b->isIsFirstTimeHit()) {
      b->setCurrDuration(b->getCurrDuration() + dt);
    }
    if (b->getCurrDuration() > b->getDuration()) {
      i = m_bullets.erase(i);
      delete b;
    } else {
      ++i;
    }
  }
}

Bullet*
BulletMgr::fire(vec3 fromPos,
                vec3 direction,
                float speed,
                BulletType bulletType)
{
  Bullet* bulletPtr;
  switch (bulletType) {
    case BulletType::HitScanLaser: {
      PhysicsHitResult result;
      if (PhysicsMgr::shared()->rayCastCloset(
            fromPos, fromPos + direction * 100, result)) {
        auto emitter = new ParticleEmitter(1);
        emitter->setIsLocalPos(true);
        emitter->setTex("ParticleTex/smoke_04.png");
        emitter->setSpawnRate(0.3);
        emitter->addInitModule(new ParticleInitSizeModule(0.5, 0.7));

        emitter->addInitModule(new ParticleInitLifeSpanModule(0.3, 0.3));
        emitter->addInitModule(new ParticleInitAlphaModule(0.6, 0.6));
        emitter->addUpdateModule(new ParticleUpdateColorModule(
          vec4(1.0, 1.0, 1.0, 1.0), vec4(0.8, 0.8, 1.0, 0.0)));
        emitter->addUpdateModule(new ParticleUpdateSizeModule(1.0, 1.5));
        emitter->setIsState(ParticleEmitter::State::Playing);
        emitter->setDepthBias(0.05);
        emitter->setPos(result.posInWorld);
        emitter->setIsInfinite(false);
        emitter->setBlendState(1);
        g_GetCurrScene()->addNode(emitter);

        auto line = new LaserPrimitive(fromPos, result.posInWorld, 1.5, true);
        g_GetCurrScene()->addNode(line);
        bulletPtr = new LaserBullet(line);
        bulletPtr->setDuration(0.1);

      } else {
        auto line = new LaserPrimitive(fromPos, fromPos + direction * 100, 1.5, true);
        g_GetCurrScene()->addNode(line);
        bulletPtr = new LaserBullet(line);
        bulletPtr->setDuration(0.1);
      }
    } break;
    case BulletType::HitScanTracer: {
      PhysicsHitResult result;
      if (PhysicsMgr::shared()->rayCastCloset(
            fromPos, fromPos + direction * 100, result)) {
        auto emitter = new ParticleEmitter(1);
        emitter->setIsLocalPos(true);
        emitter->setTex("ParticleTex/smoke_04.png");
        emitter->setSpawnRate(0.3);
        emitter->addInitModule(new ParticleInitSizeModule(0.5, 0.7));

        emitter->addInitModule(new ParticleInitLifeSpanModule(0.3, 0.3));
        emitter->addInitModule(new ParticleInitAlphaModule(0.6, 0.6));
        emitter->addUpdateModule(new ParticleUpdateColorModule(
          vec4(1.0, 1.0, 1.0, 1.0), vec4(0.8, 0.8, 1.0, 0.0)));
        emitter->addUpdateModule(new ParticleUpdateSizeModule(1.0, 1.5));
        emitter->setIsState(ParticleEmitter::State::Playing);
        emitter->setDepthBias(0.05);
        emitter->setPos(result.posInWorld);
        emitter->setIsInfinite(false);
        emitter->setBlendState(1);
        g_GetCurrScene()->addNode(emitter);
      }

      auto line = new LaserPrimitive(fromPos, fromPos + direction * 15, 0.3, true);
      g_GetCurrScene()->addNode(line);
      bulletPtr = new LaserBullet(line);
      bulletPtr->setDuration(0.1);
    } break;
    case BulletType::Projecttile: {
      float blockSize = 0.2;
      auto boxA = new SpherePrimitive(blockSize, 8);
      auto firePos = fromPos + direction * 0.5;
      boxA->setPos(firePos);
      auto transform = boxA->getTransform();
      auto aabb = boxA->localAABB();
      auto rigA =
        PhysicsMgr::shared()->createRigidBodySphere(1.0, transform, blockSize);
      rigA->setFriction(0.3);
      rigA->attach(boxA);
      rigA->setVelocity(direction * speed);
      rigA->setCcdMotionThreshold(1e-7);
      rigA->setCcdSweptSphereRadius(blockSize);
      rigA->rigidBody()->setCollisionFlags(
        rigA->rigidBody()->getCollisionFlags() |
        btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
      g_GetCurrScene()->addNode(boxA);
      PhysicsMgr::shared()->addRigidBody(rigA);
      bulletPtr = new ProjectileBullet(rigA);
    }
      break;
    case BulletType::PulseLaser:
  	{
      float blockSize = 0.2;
      auto line = new LaserPrimitive(fromPos, fromPos + direction * 3, 1.0,false);
      auto transform = line->getTransform();
      auto aabb = line->localAABB();
      auto rigA =
        PhysicsMgr::shared()->createRigidBodySphere(1.0, transform, blockSize);
      rigA->setFriction(0.3);
      rigA->attach(line);
      rigA->setVelocity(direction * speed);
      rigA->setCcdMotionThreshold(1e-7);
      rigA->setCcdSweptSphereRadius(blockSize);
      rigA->rigidBody()->setCollisionFlags(
		rigA->rigidBody()->getCollisionFlags() |
        btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
      g_GetCurrScene()->addNode(line);
      PhysicsMgr::shared()->addRigidBody(rigA);
      bulletPtr = new ProjectileBullet(rigA);
    }
	break;
  }
  m_bullets.emplace_back(bulletPtr);
  return bulletPtr;

}
}
