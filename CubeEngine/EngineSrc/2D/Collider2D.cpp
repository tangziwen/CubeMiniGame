#include "Collider2D.h"
#include "QuadTree2D.h"
namespace tzw
{
	Collider2D::Collider2D()
		:m_radius(16.f), m_pos(), m_parent(nullptr)
	{
	}
	Collider2D::Collider2D(float radius, vec2 newPos)
		:m_radius(radius), m_pos(newPos), m_parent(nullptr)
	{
		calculateAABB();
	}
	void Collider2D::setPos(vec2 newPos)
	{
		m_pos = newPos;
		calculateAABB();
		if(m_parent)
		{
			if(!m_parent->isCanContain(this))
			{
				m_parent->m_outerParent->updateCollider(this);
			}
		}
	}
	void Collider2D::setRadius(float newRadius) 
	{
		m_radius = newRadius; 
		calculateAABB();
		if(m_parent)
		{
			if(!m_parent->isCanContain(this))
			{
				m_parent->m_outerParent->updateCollider(this);
			}
		}
	}
	const AABB2D& Collider2D::getAABB()
	{
		return m_aabb;
	}
	void Collider2D::onCollision(Collider2D* other)
	{
		if(m_cb)
		{
			m_cb(this, other);
		}
	}
	void Collider2D::calculateAABB()
	{
		m_aabb.set(vec2(m_pos.x - m_radius, m_pos.y - m_radius), vec2(m_pos.x + m_radius, m_pos.y + m_radius));
	}
}