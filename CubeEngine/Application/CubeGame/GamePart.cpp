#include "GamePart.h"
namespace tzw
{

	Drawable3D * GamePart::getNode() const
	{
		return m_node;
	}

	void GamePart::setNode(Drawable3D * node)
	{
		m_node = node;
	}

	PhysicsShape * GamePart::getShape()
	{
		return m_shape;
	}

	bool GamePart::findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up)
	{
		return false;
	}

	void GamePart::attachTo(vec3 attachPosition, vec3 Normal, vec3 up)
	{
	}

}