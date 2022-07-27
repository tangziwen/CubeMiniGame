#include "RLCollectible.h"
#include "RLSpritePool.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLPlayerState.h"
namespace tzw
{


	RLCollectible::RLCollectible(unsigned int typeID, vec2 Pos)
		:m_typeID(typeID)
	{
		m_collider = new Collider2D(16, Pos);
		m_collider->setResponseChannel(CollisionChannel2D_Player);
		setPos(Pos);
		m_collider->m_cb = [this](Collider2D* self, Collider2D* other)
		{
			onCollision(self, other);
		};
	}

	RLCollectible::~RLCollectible()
	{
		RLCollectibleMgr::shared()->removeGraphics(m_sprite);
	}

	vec2 RLCollectible::getPos()
	{
		return m_pos;
	}

	void RLCollectible::setPos(vec2 pos)
	{
		m_pos = pos;
	}

	void RLCollectible::initGraphics()
	{
		m_sprite = RLCollectibleMgr::shared()->giveGraphics();
		m_sprite->pos = m_pos;
	}

	void RLCollectible::onCollision(Collider2D* self, Collider2D* other)
	{
		
		if(other->getUserData().m_userData)
		{
			
			switch(other->getUserData().m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				RLHero * hero = reinterpret_cast<RLHero *>(other->getUserData().m_userData);
				if(hero->getIsPlayerControll())
				{
					RLPlayerState::shared()->gainExp(800);
					m_isAlive = false;
				}
			}
				break;
			default:
			{
			
			
			}
			break;
			}
		}
	}

	bool RLCollectible::getIsAlive()
	{
		return m_isAlive;
	}

	RLCollectibleMgr::RLCollectibleMgr()
	{
		m_collectibleSpriteType = RLSpritePool::shared()->get()->addTileType("RL/Bullet.png");
	}

	void RLCollectibleMgr::addCollectible(int typeID, vec2 pos)
	{
		auto collectible = new RLCollectible(typeID, pos);
		m_collectible.push_back(collectible);
		collectible->initGraphics();
	}

	SpriteInstanceInfo* RLCollectibleMgr::giveGraphics()
	{
		SpriteInstanceInfo * info = new SpriteInstanceInfo();
		info->type = m_collectibleSpriteType;
		RLSpritePool::shared()->get()->addTile(info);
		return info;
	}

	void RLCollectibleMgr::removeGraphics(SpriteInstanceInfo* info)
	{
		RLSpritePool::shared()->get()->removeSprite(info);
	}

	void RLCollectibleMgr::tick(float dt)
	{
		for(auto iter = m_collectible.begin();iter != m_collectible.end();)
		{
			RLCollectible * collectible = (*iter);
			RLWorld::shared()->getQuadTree()->checkCollision(collectible->m_collider);
		
			if(!collectible->getIsAlive())
			{
				//ready to kill

				RLWorld::shared()->getQuadTree()->removeCollider(collectible->m_collider);
				iter = m_collectible.erase(iter);
				delete collectible;
			}
			else
			{
				++iter;
			}
		}
	}



}