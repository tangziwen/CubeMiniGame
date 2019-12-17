#pragma once
#include "Math/vec3.h"
#include "Math/Quaternion.h"
#include <functional>
class btCollisionShape;
namespace tzw
{

class PhysicsListener
{
public:
	enum class ListenerType
	{
		Normal,
		Projectile
	};
	PhysicsListener();
	virtual ~PhysicsListener() = default;
	virtual void recievePhysicsInfo(vec3 pos, Quaternion rot);
	virtual ListenerType getListenerType();
	void setListenerType(const ListenerType listenerType);
	std::function<void (vec3)> m_onHitCallBack;
private:
	ListenerType m_listenerType;
};
}
