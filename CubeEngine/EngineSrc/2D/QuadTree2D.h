#pragma once
#include "Math/vec2.h"
#include "Collider2D.h"
#include <vector>
#include "AABB2D.h"
namespace tzw
{
class QuadTree2D;
struct QuadTree2DNode
{
	vec2 m_pos;
	vec2 m_size;
	QuadTree2DNode(vec2 pos, vec2 size, int depthIndex, QuadTree2D * outerParent);
	bool addCollider_R(Collider2D * obj);
	void getRange_R(AABB2D range, std::vector<Collider2D *> & colliderList);
	bool isCanContain(Collider2D * obj);

	QuadTree2DNode * m_children[4] = {};
	std::vector<Collider2D *> m_collidersList;
	AABB2D m_aabb;
	QuadTree2D * m_outerParent = nullptr;
	int m_currDepth = 0;
};

class QuadTree2D
{
public:
	void init(vec2 origin, vec2 size);
	void addCollider(Collider2D * obj);
	void removeCollider(Collider2D * obj);
	void updateCollider(Collider2D * obj);
	void checkCollision(Collider2D * obj);
	void getRange(AABB2D range, std::vector<Collider2D *> & collider);
protected:
	QuadTree2DNode * m_rootNode;
	std::vector<Collider2D * > m_tmpColliderList;
};
}