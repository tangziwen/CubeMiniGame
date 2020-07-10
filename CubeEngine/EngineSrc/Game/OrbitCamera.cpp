#include "OrbitCamera.h"
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
namespace tzw {

const float HeightThreadHold = 0.01;
OrbitCamera::OrbitCamera()
    :collideCheck(nullptr),m_maxFallSpeed(6),m_distToside(0.25), m_isEnableGravity(true),m_speed(vec3(6,5,6)),m_rotateSpeed(vec3(0.1,0.1,0.1)),m_forward(0)
    ,m_slide(0),m_up(0),m_isFirstLoop(true)
    ,m_verticalSpeed(0),m_gravity(0.5),distToGround(1.7),m_isStopUpdate(false),m_defaultDist(5.0f)
{
    offsetToCentre = 0.6;
    m_distToFront = 0.2;
	m_dist = m_defaultDist;
    m_enableFPSFeature = true;
    collisionPackage = new ColliderEllipsoid();
    collisionPackage->eRadius = vec3(m_distToside, distToGround, m_distToFront);
	setLocalPiority(-1);
	resetDirection();
	
	m_focusNode = nullptr;
}

OrbitCamera *OrbitCamera::create(Camera *cloneObj)
{
    auto camera =new  OrbitCamera();
    camera->init(cloneObj);
    return camera;
}

bool OrbitCamera::onKeyPress(int keyCode)
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
             m_verticalSpeed = 0.2;
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

bool OrbitCamera::onKeyRelease(int keyCode)
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

bool OrbitCamera::onMouseRelease(int button, vec2 pos)
{
    return true;
}

bool OrbitCamera::onMousePress(int button, vec2 pos)
{
    //m_oldPosition = vec3(pos.x,pos.y,0.0);
    return true;
}

bool OrbitCamera::onMouseMove(vec2 pos)
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
    	m_longitude += mouseForce.x * 0.01;
    	if(m_longitude > 2.0 * 3.14)
    	{
    		m_longitude = 0.0f;
    	}else
    	if(m_longitude < 0.0)
    	{
    		m_longitude = 2.0 * 3.14;
    	}
    	m_latitude += mouseForce.y * 0.005;
    	if(m_latitude > 3.14f)
    	{
    		m_latitude = 3.14f;
    	} else
		if(m_latitude < 0.0f)
    	{
    		m_latitude = 0.0f;
    	}

    	//tlog("m_latitude%s", m_latitude);
    }
	
    m_oldPosition = newPosition;
    return true;
}

void OrbitCamera::logicUpdate(float dt)
{
	m_frustum.initFrustumFromCamera(this);
    return;
}


vec3 OrbitCamera::speed() const
{
    return m_speed;
}

void OrbitCamera::setSpeed(const vec3 &speed)
{
    m_speed = speed;
}
vec3 OrbitCamera::rotateSpeed() const
{
    return m_rotateSpeed;
}

void OrbitCamera::setRotateSpeed(const vec3 &rotateSpeed)
{
    m_rotateSpeed = rotateSpeed;
}

float OrbitCamera::getDistToGround() const
{
    return distToGround;
}

void OrbitCamera::setDistToGround(float value)
{
    distToGround = value;
}
float OrbitCamera::getGravity() const
{
    return m_gravity;
}

void OrbitCamera::setGravity(float gravity)
{
    m_gravity = gravity;
}
bool OrbitCamera::getIsEnableGravity() const
{
    return m_isEnableGravity;
}

void OrbitCamera::setIsEnableGravity(bool isEnableGravity)
{
    m_isEnableGravity = isEnableGravity;
}
float OrbitCamera::getDistToside() const
{
    return m_distToside;
}

void OrbitCamera::setDistToside(float distToside)
{
    m_distToside = distToside;
}
float OrbitCamera::getMaxFallSpeed() const
{
    return m_maxFallSpeed;
}

void OrbitCamera::setMaxFallSpeed(float maxFallSpeed)
{
    m_maxFallSpeed = maxFallSpeed;
}
bool OrbitCamera::getEnableFPSFeature() const
{
    return m_enableFPSFeature;
}

void OrbitCamera::setEnableFPSFeature(bool enableFPSFeature)
{
    m_enableFPSFeature = enableFPSFeature;
}

void OrbitCamera::setRotateQ(const Quaternion &rotateQ)
{
    Camera::setRotateQ(rotateQ);
}

void OrbitCamera::lookAt(vec3 pos)
{
    Camera::lookAt(pos,vec3(0.f, 1.f,0.f));
}

void OrbitCamera::init(Camera *cloneObj)
{
    setProjection(cloneObj->projection());
    setPos(cloneObj->getPos());
    setRotateE(cloneObj->getRotateE());
    setScale(vec3(1.0,1.0,1.0));
    EventMgr::shared()->addFixedPiorityListener(this);
	cloneObj->getPerspectInfo(&m_fov, &m_aspect, &m_near, &m_far);
}

void OrbitCamera::collideAndSlide(vec3 vel, vec3 gravity)
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

vec3 OrbitCamera::collideWithWorld(const vec3 &pos, const vec3 &vel, bool needSlide)
{
    float veryCloseDistance = 0.005f;
    // do we need to worry?
    if (collisionRecursionDepth > 5)
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
    if (!collisionPackage->foundCollision) {
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
        collisionPackage->intersectionPoint -= V * std::min(static_cast<float>(collisionPackage->nearestDistance), veryCloseDistance);
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

void OrbitCamera::checkCollision(ColliderEllipsoid * thePackage)
{
    std::vector<Drawable3D *> list;
    vec3 pos = thePackage->R3Position;
    AABB aabb;
    aabb.update(vec3(pos.x - 3,pos.y - 3,pos.z - 3));
    aabb.update(vec3(pos.x + 3,pos.y + 3 ,pos.z + 3));
    g_GetCurrScene()->getRange(&list, static_cast<uint32_t>(DrawableFlag::All), aabb);
	if (list.empty()) return;
    Drawable3DGroup group(&list[0],list.size());
    group.checkCollide(thePackage);
}

float OrbitCamera::getDefaultDist() const
{
	return m_defaultDist;
}

void OrbitCamera::setDefaultDist(const float defaultDist)
{
	m_defaultDist = defaultDist;
}

bool OrbitCamera::getIsMoving() const
{
    return m_isMoving;
}

void OrbitCamera::setIsMoving(bool isMoving)
{
    m_isMoving = isMoving;
}

void OrbitCamera::setFocusNode(Node* focusNode)
{
	m_focusNode = focusNode;
}
static Matrix44 targetTo(vec3 eye, vec3 target, vec3 up)
{
	Matrix44 mat;
	auto out = mat.data();
	float eyex = eye.x,
	eyey = eye.y,
	eyez = eye.z,
	upx = up.x,
	upy = up.y,
	upz = up.z;

	float z0 = eyex - target.x,
	z1 = eyey - target.y,
	z2 = eyez - target.z;

	float len = z0*z0 + z1*z1 + z2*z2;
	if (len > 0) {
	len = 1 / sqrt(len);
	z0 *= len;
	z1 *= len;
	z2 *= len;
  }

  float x0 = upy * z2 - upz * z1,
      x1 = upz * z0 - upx * z2,
      x2 = upx * z1 - upy * z0;

  len = x0*x0 + x1*x1 + x2*x2;
  if (len > 0) {
    len = 1 / sqrt(len);
    x0 *= len;
    x1 *= len;
    x2 *= len;
  }

	out[0] = x0;
	out[1] = x1;
	out[2] = x2;
	out[3] = 0;
	out[4] = z1 * x2 - z2 * x1;
	out[5] = z2 * x0 - z0 * x2;
	out[6] = z0 * x1 - z1 * x0;
	out[7] = 0;
	out[8] = z0;
	out[9] = z1;
	out[10] = z2;
	out[11] = 0;
	out[12] = eyex;
	out[13] = eyey;
	out[14] = eyez;
	out[15] = 1;
	return mat;
}
Matrix44 OrbitCamera::getTransform()
{
	if(m_focusNode) 
	{
		m_focusNode->reCache();
		auto centrePos = m_focusNode->getTransform().getTranslation();
		vec3 dir;
		dir.x = m_dist * -1.0f * cosf(m_longitude) * sinf(m_latitude);
		dir.z = m_dist * -1.0f * sinf(m_longitude) * sinf(m_latitude);
		dir.y = m_dist * cosf(m_latitude);
		auto camPos = centrePos + dir;
		auto resultMat = targetTo(camPos, centrePos, vec3(0, 1, 0));
		
		return resultMat;
	}else
	{
		Matrix44 mat;
		mat.setToIdentity();
		return mat;
	}
}

void OrbitCamera::resetDirection()
{
	m_longitude = 0.0;
	m_latitude = 60.0 * 3.14 / 180.0f;
	m_dist = m_defaultDist;
}

void OrbitCamera::zoom(float dist)
{
	m_dist += dist;
	m_dist = std::max(std::min(m_dist, 20.0f), 1.5f);
}
} // namespace tzw

