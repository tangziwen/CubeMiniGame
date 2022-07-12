#pragma once
#include "Math/vec2.h"
#include "AABB2D.h"
#include <functional>
namespace tzw
{
	struct QuadTree2DNode;
	class Collider2D
	{
	public:
		Collider2D(float radius, vec2 newPos);
		void setPos(vec2 newPos);
		float getRadius(){return m_radius;}
		vec2 getPos() {return m_pos;}
		void setParent(QuadTree2DNode * newParent) {m_parent = newParent;}
		QuadTree2DNode * getParent() {return m_parent;}
		const AABB2D & getAABB();
		void onCollision(Collider2D * other);
		std::function<void (Collider2D *, Collider2D * )> m_cb;
		bool getIsCollisionEnable () {return m_isCollisionEnable;}
		void setIsCollisionEnable (bool newVal) {m_isCollisionEnable = newVal;}
	protected:
		void calculateAABB();
		vec2 m_pos;
		float m_radius;
		QuadTree2DNode * m_parent;
		AABB2D m_aabb;
		bool m_isCollisionEnable = true;
		
	};

}