#include "FPSCamera.h"
#include "EngineSrc/Engine/Engine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "EngineSrc/Scene/SceneMgr.h"

#include "Chunk.h"
#include <iostream>
#include <time.h>
#include "EngineSrc/Math/t_Sphere.h"
#include "EngineSrc/Math/Plane.h"
#include <cmath>

namespace tzw {

const float HeightThreadHold = 0.01;
FPSCamera::FPSCamera()
    :collideCheck(nullptr),m_maxFallSpeed(6),m_distToside(0.25), m_isEnableGravity(true),m_forward(0),m_slide(0)
    ,m_speed(vec3(6,2,6)),m_rotateSpeed(vec3(0.1,0.1,0.1)),m_isFirstLoop(true)
    ,m_verticalSpeed(0),m_gravity(0.5),distToGround(0.7),m_isStopUpdate(false)
{
    offsetToCentre = 0.6;
    m_distToFront = 0.2;
    m_enableFPSFeature = true;
    setUseCustomFrustumUpdate(true);
    collisionPackage = new ColliderEllipsoid();
    collisionPackage->eRadius = vec3(m_distToside, distToGround, m_distToFront);
}

FPSCamera *FPSCamera::create(Camera *cloneObj)
{
    auto camera =new  FPSCamera();
    camera->init(cloneObj);
    return camera;
}

bool FPSCamera::onKeyPress(int keyCode)
{
    switch(keyCode)
    {
    case GLFW_KEY_W:
        m_forward = 1;
        break;
    case GLFW_KEY_A:
        m_slide = -1;
        break;
    case GLFW_KEY_S:
        m_forward = -1;
        break;
    case GLFW_KEY_D:
        m_slide = 1;
        break;
    case GLFW_KEY_Q:
        m_isStopUpdate = !m_isStopUpdate;
        break;
    case GLFW_KEY_SPACE:
        if(m_isEnableGravity)//now falling
        {
            m_verticalSpeed = 0.2;
        }
        break;
    }
    return true;
}

bool FPSCamera::onKeyRelease(int keyCode)
{
    switch(keyCode)
    {
    case GLFW_KEY_W:
        m_forward = 0;
        break;
    case GLFW_KEY_A:
        m_slide = 0;
        break;
    case GLFW_KEY_S:
        m_forward = 0;
        break;
    case GLFW_KEY_D:
        m_slide = 0;
        break;
    case GLFW_KEY_F:
    {
        std::vector<Drawable3D *> list;
        auto pos = this->getPos();
        AABB aabb;
        aabb.update(vec3(pos.x -10,pos.y- 10,pos.z - 10));
        aabb.update(vec3(pos.x +10,pos.y + 10 ,pos.z + 10));
        g_GetCurrScene()->getRange(&list,aabb);
        Drawable3DGroup group(&list[0],list.size());
        Ray ray(pos,this->getForward());
        vec3 hitPoint;
        auto chunk = static_cast<Chunk *>(group.hitByRay(ray,hitPoint));
        if(chunk)
        {
            auto before = clock();
            chunk->deformAround(hitPoint, -0.2);
            auto delta = clock() - before;
        }
    }
        break;
    case GLFW_KEY_E:
    {
        std::vector<Drawable3D *> list;
        auto pos = this->getPos();
        AABB aabb;
        aabb.update(vec3(pos.x -10,pos.y- 10,pos.z - 10));
        aabb.update(vec3(pos.x +10,pos.y + 10 ,pos.z + 10));
        g_GetCurrScene()->getRange(&list,aabb);
        Drawable3DGroup group(&list[0],list.size());
        Ray ray(pos,this->getForward());
        vec3 hitPoint;
        auto chunk = static_cast<Chunk *>(group.hitByRay(ray,hitPoint));
        if(chunk)
        {
            chunk->deformAround(hitPoint, 0.2);
        }
    }
        break;
    }
    return true;
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
    if(m_isFirstLoop)
    {
        m_oldPosition = vec3(pos.x,pos.y,0.0);
        m_isFirstLoop = false;
    }

    auto newPosition = vec3(pos.x,pos.y,0.0);

    if (true)
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
        auto deltaRot = vec3(mouseForce.y * 0.1,-1 * mouseForce.x * 0.1, 0);
        setRotateE(getRotateE()  + deltaRot);
    }
    m_oldPosition = newPosition;
    return true;
}

void FPSCamera::logicUpdate(float dt)
{
    if(!m_enableFPSFeature) return;
    auto m = getTransform().data();
    vec3 forwardDirection,rightDirection;
    if(m_isEnableGravity)
    {
        forwardDirection = vec3(-1*m[8],0,-1*m[10]);
        rightDirection = vec3(m[0],0,m[2]);
    }else
    {
        forwardDirection = vec3(-1*m[8],-1*m[9],-1*m[10]);
        rightDirection = vec3(m[0],m[1],m[2]);
    }
    forwardDirection.normalize();
    rightDirection.normalize();
    auto pos = this->getPos();

    std::vector<Drawable3D *> list;
    AABB aabb;
    aabb.update(vec3(pos.x -3,pos.y- 10,pos.z - 3));
    aabb.update(vec3(pos.x +3,pos.y + 10 ,pos.z + 3));
	
    g_GetCurrScene()->getRange(&list,aabb);
	Drawable3DGroup group;
	if (!list.empty())
	{
		group.init(&list[0], list.size());
	}
    AABB playerAABB;
    vec3 overLap;
    vec3 totalSpeed = forwardDirection*dt*m_speed.z *m_forward;
    totalSpeed += rightDirection*dt*m_speed.x*m_slide;

    if(!group.hitByAABB(playerAABB,overLap) && false)//if no collid just go through TZW just for speed.
    {
        vec3 test = pos + totalSpeed;
        pos = test;
        setPos(pos);
    }else
    {
        vec3 userSpeed = vec3(totalSpeed.x, 0, totalSpeed.z);
        vec3 gravityVelocity;
        if(m_isEnableGravity)
        {
            Ray ray(m_pos,vec3(0, -1, 0));
            vec3 hitPoint;
            if(group.hitByRay(ray,hitPoint) && hitPoint.distance(m_pos) <= (distToGround + offsetToCentre + 0.05) && m_verticalSpeed < 0.0f)
            {
                m_verticalSpeed = 0.0;
            }else
            {
                if(m_verticalSpeed <= -0.5)
                {
                    m_verticalSpeed = -0.5;
                }
                else
                {
                    m_verticalSpeed -= 0.005;
                }
            }
            gravityVelocity = vec3(0,m_verticalSpeed,0);
        }
        collideAndSlide(userSpeed,gravityVelocity);
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
bool FPSCamera::getIsEnableGravity() const
{
    return m_isEnableGravity;
}

void FPSCamera::setIsEnableGravity(bool isEnableGravity)
{
    m_isEnableGravity = isEnableGravity;
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


void FPSCamera::reCache()
{
    Node::reCache();
    if(!m_isStopUpdate)
    {
        Camera::updateFrustum();
    }
}

void FPSCamera::setRotateQ(const Quaternion &rotateQ)
{
    Camera::setRotateQ(rotateQ);
}

void FPSCamera::lookAt(vec3 pos)
{
    Camera::lookAt(pos,vec3(0.f, 1.f,0.f));
    //    auto dir =  pos - m_pos;
    //    dir.normalize();
    //    auto pitch = TbaseMath::Radius2Ang(asin(dir.y));
    //    auto yaw = TbaseMath::Radius2Ang(acosf(dir.x / cosf(pitch)));
    //    setRotateE(vec3(pitch, yaw, 0.0));
    std::cout<< "the rotation is "<<getRotateE().getStr()<<std::endl;
}

void FPSCamera::init(Camera *cloneObj)
{
    setProjection(cloneObj->projection());
    setPos(cloneObj->getPos());
    setRotateE(cloneObj->getRotateE());
    setScale(vec3(1.0,1.0,1.0));
    EventMgr::shared()->addFixedPiorityListener(this);
}

void FPSCamera::collideAndSlide(vec3 vel, vec3 gravity)
{
    // Do collision detection:
    collisionPackage->R3Position = m_pos - vec3(0, offsetToCentre, 0);
    collisionPackage->R3Velocity = vel;
    // calculate position and velocity in eSpace
    vec3 eSpacePosition = collisionPackage->R3Position / collisionPackage->eRadius;
    vec3 eSpaceVelocity = collisionPackage->R3Velocity / collisionPackage->eRadius;
    // Iterate until we have our final position.
    collisionRecursionDepth = 0;
    vec3 finalPosition = collideWithWorld(eSpacePosition,eSpaceVelocity);


    //do it again for gravity
    collisionPackage->R3Position = finalPosition * collisionPackage->eRadius;
    collisionPackage->R3Velocity = gravity;
    eSpaceVelocity = collisionPackage->R3Velocity / collisionPackage->eRadius;
    collisionRecursionDepth = 0;
    finalPosition = collideWithWorld(finalPosition,eSpaceVelocity, false);

    //now set the position
    // Convert final result back to R3:
    finalPosition = finalPosition * collisionPackage->eRadius;
    if (finalPosition.distance(getPos()) > 0.001)
    {
        m_isMoving = true;
    }else
    {
        m_isMoving = false;
    }
    setPos(finalPosition + vec3(0, offsetToCentre,0));
}

vec3 FPSCamera::collideWithWorld(const vec3 &pos, const vec3 &vel, bool needSlide)
{
    float veryCloseDistance = 0.005f;
    // do we need to worry?
    if (collisionRecursionDepth>5)
        return pos;
    // Ok, we need to worry:
    collisionPackage->velocity = vel;

    collisionPackage->normalizedVelocity = vel;
    collisionPackage->normalizedVelocity.normalize();
    collisionPackage->basePoint = pos;
    collisionPackage->foundCollision = false;
    // Check for collision (calls the collision routines)
    // Application specific!!
    checkCollision(collisionPackage);
    // If no collision we just move along the velocity
    if (collisionPackage->foundCollision == false) {
        return pos + vel;
    }
    // *** Collision occured ***
    // The original destination point
    vec3 destinationPoint = pos + vel;
    vec3 newBasePoint = pos;
    // only update if we are not already very close
    // and if so we only move very close to intersection..not
    // to the exact spot.
    if (collisionPackage->nearestDistance>=veryCloseDistance)
    {
        vec3 V = vel;
        V.setLength(collisionPackage->nearestDistance - veryCloseDistance);
        newBasePoint = collisionPackage->basePoint + V;
        // Adjust polygon intersection point (so sliding
        // plane will be unaffected by the fact that we
        // move slightly less than collision tells us)
        V.normalize();
        collisionPackage->intersectionPoint -= V * std::min((float)collisionPackage->nearestDistance, veryCloseDistance);
//        collisionPackage->intersectionPoint -= V * veryCloseDistance;
    }
    // Determine the sliding plane
    vec3 slidePlaneOrigin =

            collisionPackage->intersectionPoint;
    vec3 slidePlaneNormal =
            newBasePoint-collisionPackage->intersectionPoint;
    slidePlaneNormal.normalize();
    Plane slidingPlane(slidePlaneNormal , slidePlaneOrigin);
    // Again, sorry about formatting.. but look carefully ;)
    vec3 newDestinationPoint = destinationPoint - slidePlaneNormal * slidingPlane.dist2Plane(destinationPoint);
    // Generate the slide vector, which will become our new
    // velocity vector for the next iteration
    vec3 newVelocityVector = newDestinationPoint - collisionPackage->intersectionPoint;
    // Recurse:
    // dont recurse if the new velocity is very small
    if (newVelocityVector.length() < veryCloseDistance || !needSlide) {
        return newBasePoint;
    }
    collisionRecursionDepth++;
    return collideWithWorld(newBasePoint,newVelocityVector);
}

void FPSCamera::checkCollision(ColliderEllipsoid * thePackage)
{
    std::vector<Drawable3D *> list;
    vec3 pos = thePackage->R3Position;
    AABB aabb;
    aabb.update(vec3(pos.x - 10,pos.y - 10,pos.z - 10));
    aabb.update(vec3(pos.x + 10,pos.y + 10 ,pos.z + 10));
    g_GetCurrScene()->getRange(&list,aabb);
	if (list.empty()) return;
    Drawable3DGroup group(&list[0],list.size());
    group.checkCollide(thePackage);
}

bool FPSCamera::getIsMoving() const
{
    return m_isMoving;
}

void FPSCamera::setIsMoving(bool isMoving)
{
    m_isMoving = isMoving;
}
} // namespace tzw

