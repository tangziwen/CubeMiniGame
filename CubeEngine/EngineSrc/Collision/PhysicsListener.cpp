#include "PhysicsListener.h"
namespace tzw
{
	PhysicsListener::PhysicsListener():m_listenerType(ListenerType::Normal)
	{
	}

	void PhysicsListener::recievePhysicsInfo(vec3 pos, Quaternion rot)
	{
		
	}

	PhysicsListener::ListenerType PhysicsListener::getListenerType()
	{
		return m_listenerType;
	}

	void PhysicsListener::setListenerType(const ListenerType listenerType)
	{
		m_listenerType = listenerType;
	}
}
