#include "FPSCamera.h"
#include "EngineSrc/Engine/Engine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "EngineSrc/Scene/SceneMgr.h"

#include <iostream>
#include <time.h>
#include "EngineSrc/Math/t_Sphere.h"
#include "EngineSrc/Math/Plane.h"
#include <cmath>
#include "CubeGame/GameUISystem.h"
#include <algorithm>
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "Collision/PhysicsMgr.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btConvexShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "btBulletCollisionCommon.h"

namespace tzw {


const float HeightThreadHold = 0.01;
FPSCamera::FPSCamera(bool isOpenPhysics)
    :collideCheck(nullptr),m_maxFallSpeed(6),m_distToside(0.25), m_isEnableGravity(true),m_speed(vec3(3.5f,0.0f,3.5f)),m_rotateSpeed(vec3(0.1,0.1,0.1)),m_forward(0)
    ,m_slide(0),m_up(0),m_isFirstLoop(true),m_isOpenPhysics(isOpenPhysics)
    ,m_verticalSpeed(0),m_gravity(0.5),distToGround(1.7),m_isStopUpdate(false),
	m_onHitGround(nullptr)
{
    offsetToCentre = 0.6;
    m_distToFront = 0.2;
    m_enableFPSFeature = true;
    setUseCustomFrustumUpdate(true);
    collisionPackage = new ColliderEllipsoid();
    collisionPackage->eRadius = vec3(m_distToside, distToGround, m_distToFront);
	setLocalPiority(-1);


	if(m_isOpenPhysics) {
		m_capsuleHigh = 1.2f;
		btCapsuleShape *shape = new btCapsuleShape(0.25f, m_capsuleHigh);

		this->m_ghost2 = new btPairCachingGhostObject();
		//auto shape = PhysicsMgr::shared()->createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		this->m_ghost2->setCollisionShape(shape);

		this->m_ghost2->setFriction(0.0f);
		this->m_ghost2->setCollisionFlags(btCollisionObject::CollisionFlags::CF_CHARACTER_OBJECT);

		
		this->m_character = new btKinematicCharacterController(this->m_ghost2, static_cast<btConvexShape*>(shape), 0.2f, btVector3(0.0,1.0,0.0));

	    m_character->setGravity(btVector3(0, -9.8f, 0));
	    m_character->setLinearDamping(0.2f);
	    m_character->setAngularDamping(0.2f);
	    m_character->setStepHeight(0.4f);
	    m_character->setMaxJumpHeight(3.5);
	    m_character->setMaxSlope(46.f * 3.141592653 / 180.0f);
	    m_character->setJumpSpeed(7.0f);
	    m_character->setFallSpeed(55.0f);
		m_character->onHitLand = std::bind(&FPSCamera::onHitGroundImp, this);
		this->m_ghost2->setUserPointer(static_cast<PhysicsListener * >(this));
		this->m_ghost2->setUserIndex(1);
		this->m_ghost2->setCcdSweptSphereRadius(0.5f);
		this->m_ghost2->setCcdMotionThreshold(0.00001);
		PhysicsMgr::shared()->getDynamicsWorld()->addCollisionObject(this->m_ghost2, btBroadphaseProxy::AllFilter, btBroadphaseProxy::AllFilter);
		PhysicsMgr::shared()->getDynamicsWorld()->addAction(this->m_character);
	}
}

FPSCamera *FPSCamera::create(Camera *cloneObj, bool isOpenPhysics)
{
    auto camera =new  FPSCamera(isOpenPhysics);
    camera->init(cloneObj);
    return camera;
}

bool FPSCamera::onKeyPress(int keyCode)
{
	if (GameUISystem::shared()->isAnyShow())
		return false;
    switch(keyCode)
    {
    case TZW_KEY_W:
        m_forward = 1;
        break;
    case TZW_KEY_A:
        m_slide = -1;
        break;
    case TZW_KEY_S:
        m_forward = -1;
        break;
    case TZW_KEY_D:
        m_slide = 1;
        break;
    case TZW_KEY_Q:
        m_isStopUpdate = !m_isStopUpdate;
        break;
    case TZW_KEY_SPACE:
        if(m_isEnableGravity)//now falling
        {
        	if(m_isOpenPhysics)
        	{
        		m_character->jump(btVector3(0, 0.0f, 0));
        	}
            
        }
		else
		{
			m_up = 1;
		}
        break;
	case TZW_KEY_LEFT_CONTROL:
		{
			if(m_isEnableGravity)
			{

			}
			else
			{
				m_up = -1;
			}
		}
		break;
    default: ;
    }

    return false;
}

bool FPSCamera::onKeyRelease(int keyCode)
{
	if (GameUISystem::shared()->isAnyShow())
		return false;
    switch(keyCode)
    {
    case TZW_KEY_W:
        m_forward = 0;
        break;
    case TZW_KEY_A:
        m_slide = 0;
        break;
    case TZW_KEY_S:
        m_forward = 0;
        break;
    case TZW_KEY_D:
        m_slide = 0;
        break;
	case TZW_KEY_SPACE:
		{
			if(m_isEnableGravity)
			{

			}
			else
			{
				m_up = 0;
			}
		}
	break;
	case TZW_KEY_LEFT_CONTROL:
		{
			if(m_isEnableGravity)
			{

			}
			else
			{
				m_up = 0;
			}
		}
		break;
    }
    return false;
}

bool FPSCamera::onMouseRelease(int button, vec2 pos)
{
    return true;
}

bool FPSCamera::onMousePress(int button, vec2 pos)
{
    //m_oldPosition = vec3(pos.x,pos.y,0.0);
    return true;
}

bool FPSCamera::onMouseMove(vec2 pos)
{
	auto newPosition = vec3(pos.x,pos.y,0.0);
	if (g_GetCurrScene()->defaultCamera() != this) 
	{
		m_oldPosition = newPosition;
		return false;
	}
    if(m_isFirstLoop)
    {
        m_oldPosition = vec3(pos.x,pos.y,0.0);
        m_isFirstLoop = false;
    }

    if (!GameUISystem::shared()->isAnyShow())
    {
        auto mouseForce = newPosition - m_oldPosition;
        if(std::abs(mouseForce.x)<2)
        {
            mouseForce.x = 0;
        }
        if(std::abs(mouseForce.y)<2)
        {
            mouseForce.y = 0;
        }
     //   auto deltaRot = vec3(mouseForce.y * 0.1, mouseForce.x * -1 * 0.1, 0);
    	//setRotateE(getRotateE()  + deltaRot);
    	Quaternion q1,q2;
    	q1.fromAxisAngle(vec3(1, 0, 0), mouseForce.y * 0.1);
    	q1.normalize();
    	q2.fromAxisAngle(vec3(0, 1, 0), mouseForce.x * -1 * 0.1);
    	auto result = q2 * getRotateQ() * q1;
    	result.normalize();
       setRotateQ(result);
    }
    m_oldPosition = newPosition;
    return false;
}
vec3 FPSCamera::getTotalSpeed()
{
    auto m = getTransform().data();
    vec3 forwardDirection,rightDirection, upDirction;
	forwardDirection = vec3( m[8], 0, m[10]);
	rightDirection = vec3(m[0], 0, m[2]);
	upDirction = vec3(0, 1, 0);
    forwardDirection.normalize();
    rightDirection.normalize();
	vec3 totalSpeed = forwardDirection.normalized()* m_speed.z *m_forward * -1;
    totalSpeed += rightDirection.normalized() *m_speed.x*m_slide;
	totalSpeed += upDirction * m_speed.y * m_up;
	return totalSpeed;
}

void FPSCamera::pausePhysics()
{
	PhysicsMgr::shared()->getDynamicsWorld()->removeCollisionObject(this->m_ghost2);
	PhysicsMgr::shared()->getDynamicsWorld()->removeAction(this->m_character);
}

void FPSCamera::resumePhysics()
{
	PhysicsMgr::shared()->getDynamicsWorld()->addCollisionObject(this->m_ghost2, btBroadphaseProxy::AllFilter, btBroadphaseProxy::AllFilter);
	PhysicsMgr::shared()->getDynamicsWorld()->addAction(this->m_character);
}

void FPSCamera::logicUpdate(float dt)
{
    if(!m_enableFPSFeature) return;
	auto totalSpeed = getTotalSpeed();
	if(abs(totalSpeed.x) > 0.0001 || abs(totalSpeed.z) > 0.0001)
	{
		m_isMoving = true;
	}else
	{
		m_isMoving = false;
	}
	if(m_isOpenPhysics)
	{
		m_character->setWalkDirection(btVector3(totalSpeed.x, m_up * 5, totalSpeed.z) * dt);
	}
}


vec3 FPSCamera::speed() const
{
    return m_speed;
}

void FPSCamera::setSpeed(const vec3 &speed)
{
    m_speed = speed;
}

void FPSCamera::recievePhysicsInfo(vec3 pos, Quaternion rot)
{
	if(m_isOpenPhysics) 
	{
		auto originPos = m_ghost2->getWorldTransform().getOrigin();
		auto up = vec3(0, 1, 0);//m_ghost2->getWorldTransform().getBasis().getRow(1);
		auto centerPoint = vec3(originPos.getX(), originPos.getY(), originPos.getZ());
		setPos(centerPoint + vec3(up.getX(), up.getY(), up.getZ()) * (distToGround - m_capsuleHigh / 2.0 - 0.3f));
	}

}

vec3 FPSCamera::rotateSpeed() const
{
    return m_rotateSpeed;
}

void FPSCamera::setRotateSpeed(const vec3 &rotateSpeed)
{
    m_rotateSpeed = rotateSpeed;
}

float FPSCamera::getDistToGround() const
{
    return distToGround;
}

void FPSCamera::setDistToGround(float value)
{
    distToGround = value;
}
float FPSCamera::getGravity() const
{
    return m_gravity;
}

void FPSCamera::setGravity(float gravity)
{
    m_gravity = gravity;
}

void FPSCamera::setCamPos(const vec3& pos)
{
	Node::setPos(pos);
	if(m_isOpenPhysics)
	{
		btTransform mat;
		mat.setIdentity();
		auto m =mat; //m_ghost2->getWorldTransform();
		m.setOrigin(btVector3(pos.x, pos.y, pos.z));
		m_ghost2->setWorldTransform(m);
	}
}

bool FPSCamera::getIsEnableGravity() const
{
    return m_isEnableGravity;
}

void FPSCamera::setIsEnableGravity(bool isEnableGravity)
{
    m_isEnableGravity = isEnableGravity;
	if(m_isOpenPhysics)
	{
		m_character->setEnableGravity(m_isEnableGravity);
	}
}
	
float FPSCamera::getDistToside() const
{
    return m_distToside;
}

void FPSCamera::setDistToside(float distToside)
{
    m_distToside = distToside;
}
float FPSCamera::getMaxFallSpeed() const
{
    return m_maxFallSpeed;
}

void FPSCamera::setMaxFallSpeed(float maxFallSpeed)
{
    m_maxFallSpeed = maxFallSpeed;
}
bool FPSCamera::getEnableFPSFeature() const
{
    return m_enableFPSFeature;
}

void FPSCamera::setEnableFPSFeature(bool enableFPSFeature)
{
    m_enableFPSFeature = enableFPSFeature;
}

void FPSCamera::lookAt(vec3 pos)
{
    Camera::lookAt(pos,vec3(0.f, 1.f,0.f));
}

void FPSCamera::init(Camera *cloneObj)
{
    setProjection(cloneObj->projection());
    setPos(cloneObj->getPos());
    setRotateE(cloneObj->getRotateE());
    setScale(vec3(1.0,1.0,1.0));
    EventMgr::shared()->addFixedPiorityListener(this);
	cloneObj->getPerspectInfo(&m_fov, &m_aspect, &m_near, &m_far);
}

void FPSCamera::onHitGroundImp()
{
	if(m_onHitGround)
	{
		m_onHitGround();
	}
}

bool FPSCamera::isIsOpenPhysics() const
{
	return m_isOpenPhysics;
}

void FPSCamera::setIsOpenPhysics(const bool isOpenPhysics)
{
	m_isOpenPhysics = isOpenPhysics;
}

bool FPSCamera::getIsMoving() const
{
    return m_isMoving;
}

void FPSCamera::setIsMoving(bool isMoving)
{
    m_isMoving = isMoving;
}

bool FPSCamera::isOnGround() const
{
	return m_character->onGround();
}
} // namespace tzw

