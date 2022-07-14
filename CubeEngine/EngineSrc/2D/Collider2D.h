#pragma once
#include "Math/vec2.h"
#include "AABB2D.h"
#include <functional>
#include <cstdint>
namespace tzw
{
	struct UserDataWrapper
	{
		UserDataWrapper(void * userData, int tag):m_userData(userData), m_tag(tag){};
		UserDataWrapper():m_userData(nullptr), m_tag(0){};
		void * m_userData = nullptr;
		int m_tag = 0;
	};
	enum CollisionChannel2D : uint16_t
	{
		CollisionChannel2D_Player = 1 << 1,
		CollisionChannel2D_Bullet = 1 << 2,
		CollisionChannel2D_Entity = 1 << 3,
	};
	struct QuadTree2DNode;
	class Collider2D
	{
	public:
		Collider2D();
		Collider2D(float radius, vec2 newPos);
		~Collider2D();
		void setPos(vec2 newPos);
		float getRadius(){return m_radius;}
		void setRadius(float newRadius);
		vec2 getPos() {return m_pos;}
		void setParent(QuadTree2DNode * newParent) {m_parent = newParent;}
		QuadTree2DNode * getParent() {return m_parent;}
		const AABB2D & getAABB();
		void onCollision(Collider2D * other);
		std::function<void (Collider2D *, Collider2D * )> m_cb;
		bool getIsCollisionEnable () {return m_isCollisionEnable;}
		void setIsCollisionEnable (bool newVal) {m_isCollisionEnable = newVal;}
		uint16_t getSourceChannel() {return m_sourceChannel;}
		uint16_t getResponseChannel() {return m_responseChannel;}
		void setSourceChannel(uint16_t channel) {m_sourceChannel = channel;}
		void setResponseChannel(uint16_t flags) {m_responseChannel = flags;}
		void setUserData(UserDataWrapper data) {m_userData = data;};
		UserDataWrapper getUserData() {return m_userData;};
	protected:
		void calculateAABB();
		vec2 m_pos;
		float m_radius;
		QuadTree2DNode * m_parent;
		AABB2D m_aabb;
		bool m_isCollisionEnable = true;
		uint16_t m_sourceChannel = CollisionChannel2D_Entity;
		uint16_t m_responseChannel = CollisionChannel2D_Entity;
		UserDataWrapper m_userData ;
		
	};

}