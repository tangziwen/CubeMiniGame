#include "MoveBy.h"
#include "../Base/Node.h"
namespace tzw {

	MoveBy::MoveBy(float duration,vec3 speed)
	{
		setDuration(duration);
		m_speed = speed;
	}

	void MoveBy::step(Node *node, float dt)
	{
		node->setPos(node->getPos() + m_speed * dt);
	}

	void MoveBy::final(Node *node)
	{

	}

	MoveBy::~MoveBy()
	{

	}

} // namespace tzw
