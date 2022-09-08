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
	Collider2D::~Collider2D()
	{
		if(m_parent)
		{
			m_parent->m_outerParent->removeCollider(this);
		}
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
	AABB2D& Collider2D::getAABB()
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
	void Collider2D::tick(float dt)
	{
		if(m_velocity.length() > 0.0001)
		{
			//caps to max speed
			if(m_velocity.length() > m_maxSpeed)
			{
				m_velocity = m_velocity.normalized();
				m_velocity *= m_maxSpeed;
			
			}
			setPos(m_pos + m_velocity * dt);

			//apply friction
			if(m_friction > 0)
			{
				vec2 moveDir = m_velocity.normalized();
				vec2 firctionMove = moveDir * -1 * m_friction * dt;
				m_velocity += firctionMove;
				if(vec2::DotProduct(firctionMove, m_velocity) > 0)//move backward,over shoot!
				{
					m_velocity = vec2();
				}
			}

		}
		else
		{
			m_velocity = vec2();
		}

	}
	void Collider2D::calculateAABB()
	{
		m_aabb.set(vec2(m_pos.x - m_radius, m_pos.y - m_radius), vec2(m_pos.x + m_radius, m_pos.y + m_radius));
	}
}