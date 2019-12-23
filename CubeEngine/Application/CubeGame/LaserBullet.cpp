#include "LaserBullet.h"
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
	LaserBullet::LaserBullet(LaserPrimitive* laser): m_hitSfx(nullptr)
	{
		m_laser = laser;
		m_isFirstTimeHit = false;
	}

	LaserBullet::~LaserBullet()
	{
		m_laser->removeFromParent();
		delete m_laser;
	}

}

