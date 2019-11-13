#pragma once
#include "Math/vec3.h"
#include "Math/Quaternion.h"
class btCollisionShape;
namespace tzw
{

class PhysicsListener
	{
	public:
		virtual ~PhysicsListener() = default;
		virtual void recievePhysicsInfo(vec3 pos, Quaternion rot);
	};
}
