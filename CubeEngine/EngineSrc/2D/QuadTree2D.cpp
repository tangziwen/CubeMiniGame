#include "QuadTree2D.h"
namespace tzw
{
	QuadTree2DNode::QuadTree2DNode(vec2 pos, vec2 size, int depthIndex, QuadTree2D * outerParent):
		m_pos(pos), m_size(size), m_outerParent(outerParent)
	{
		m_currDepth = depthIndex;
		m_aabb.set(pos, pos + size);
		if(depthIndex < 4)
		{
			//tb
			m_children[0] = new QuadTree2DNode(m_pos, vec2(m_size.x / 2, m_size.y / 2), depthIndex + 1, outerParent);
			//lt
			m_children[1] = new QuadTree2DNode(vec2(m_pos.x, m_pos.y + m_size.y / 2), vec2(m_size.x / 2, m_size.y / 2), depthIndex + 1, outerParent);
			//rb
			m_children[2] = new QuadTree2DNode(vec2(m_pos.x + m_size.x / 2, m_pos.y), vec2(m_size.x / 2, m_size.y / 2), depthIndex + 1, outerParent);
			//rt
			m_children[3] = new QuadTree2DNode(vec2(m_pos.x + m_size.x / 2, m_pos.y + m_size.y / 2), vec2(m_size.x / 2, m_size.y / 2), depthIndex + 1, outerParent);
		}
	}

	bool QuadTree2DNode::addCollider_R(Collider2D* obj)
	{
		if(m_children[0])
		{
			for(int i = 0; i < 4; i++)
			{
				if(m_children[i]->isCanContain(obj))
				{
					if(m_children[i]->addCollider_R(obj))
						return true;
				}
			}
		}

		if(isCanContain(obj))
		{
			if(obj->getParent())
			{
				printf("cao");
			}
			m_collidersList.push_back(obj);
			obj->setParent(this);
			return true;
		}
		else
		{
			return false;
		}
	}

	void QuadTree2DNode::getRange_R(AABB2D range, std::vector<Collider2D*>& colliderList)
	{
		if(m_aabb.isIntersect(range))
		{
			for(Collider2D * collider : m_collidersList)
			{
				if(collider->getAABB().isIntersect(range))
				{
					colliderList.push_back(collider);
				}
				
			}
			if(m_children[0])
			{
				for(int i = 0; i < 4; i++)
				{
					m_children[i]->getRange_R(range, colliderList);
				}
			}

		}
	}

	bool QuadTree2DNode::isCanContain(Collider2D* obj)
	{
		return m_aabb.isCanCotain(obj->getAABB());
	}


	void QuadTree2D::init(vec2 origin, vec2 size)
	{
		m_rootNode = new QuadTree2DNode(origin, size, 0, this);

	}
	void QuadTree2D::addCollider(Collider2D* obj)
	{
		bool isAdded = m_rootNode->addCollider_R(obj);
		if(isAdded)
			m_colliderList.push_back(obj);
	}
	void QuadTree2D::removeCollider(Collider2D* obj)
	{
		removeCollider_imp(obj);
		auto iter = std::find(m_colliderList.begin(), m_colliderList.end(), obj);
		if(iter != m_colliderList.end())
		{
			m_colliderList.erase(iter);
		}
	}
	void QuadTree2D::updateCollider(Collider2D* obj)
	{
		removeCollider_imp(obj);
		m_rootNode->addCollider_R(obj);
	}

	void QuadTree2D::checkCollision(Collider2D* obj)
	{
		if(!obj->getIsCollisionEnable()) return;
		m_tmpColliderList.clear();
		m_rootNode->getRange_R(obj->getAABB(), m_tmpColliderList);
		for(Collider2D * otherCollider: m_tmpColliderList)
		{
			if(obj != otherCollider)
			{
				if(!otherCollider->getIsCollisionEnable()) continue;
				
				vec2 dir = obj->getPos() - otherCollider->getPos();
				float dist = obj->getRadius() + otherCollider->getRadius();
				float currDist = dir.length();
				if(currDist < dist)
				{
					float diff = dist - currDist;
					diff += 0.01f;
					//overlap resolve
					bool isBullet = obj->getSourceChannel() & CollisionChannel2D_Bullet || otherCollider->getSourceChannel() & CollisionChannel2D_Bullet;
					bool isItem = obj->getSourceChannel() & CollisionChannel2D_Item || otherCollider->getSourceChannel() & CollisionChannel2D_Item;
					if(isBullet || isItem)
					{
					
					}
					else
					{
						if(dir.length() > 0.1f)
						{
							dir = dir.normalized();
						}
						else
						{
							dir = vec2(1, 0);//two collider become concentric, made a new overlap resolve vector for them.
						}
						
						obj->setPos(obj->getPos() +  dir * diff * 0.5f);
						otherCollider->setPos(otherCollider->getPos() +  dir * -diff * 0.5f);
					}


					bool responseOther = otherCollider->getSourceChannel() & obj->getResponseChannel();
					bool responseSelf = obj->getSourceChannel() & otherCollider->getResponseChannel();
					if(responseOther || responseSelf)
					{
						if(responseOther)
						{
							obj->onCollision(otherCollider);
						}
						if(responseSelf)
						{
							otherCollider->onCollision(obj);
						}
					}
				}

			}		
		}
	}
	void QuadTree2D::tick(float dt)
	{
		for(Collider2D * obj : m_colliderList)
		{
			if(obj)
			{
				obj->tick(dt);
				checkCollision(obj);
			}
		}
	}
	void QuadTree2D::getRange(AABB2D range, std::vector<Collider2D*>& collider)
	{
		m_rootNode->getRange_R(range, collider);
	}
	void QuadTree2D::removeCollider_imp(Collider2D* obj)
	{
		QuadTree2DNode * node = obj->getParent();
		if(!node) return;
		auto iter = std::find(node->m_collidersList.begin(), node->m_collidersList.end(), obj);
		if(iter != node->m_collidersList.end())
		{
			node->m_collidersList.erase(iter);
		}
		obj->setParent(nullptr);
	}
}