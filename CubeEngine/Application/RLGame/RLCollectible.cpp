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
		m_collider->setSourceChannel(CollisionChannel2D_Item);
		m_collider->setResponseChannel(CollisionChannel2D_Player);
		setPos(Pos);
		m_collider->m_cb = [this](Collider2D* self, Collider2D* other)
		{
			onCollision(self, other);
		};
		if(!m_collider->getParent())
		{
			RLWorld::shared()->getQuadTree()->addCollider(m_collider);
		}
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
					onCollect(hero);
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

	void RLCollectible::setIsAlive(bool isAlive)
	{
		m_isAlive = isAlive;
	}

	void RLCollectible::onCollect(RLHero * hero)
	{
		//RLWorld::shared()->clearLevelUpPerk();
		RLPlayerState::shared()->gainExp(85);
		//hero->setPropByName<float>("MAXHP", 500);
	}

	RLCollectibleLevelUpPerk::RLCollectibleLevelUpPerk(unsigned int typeID, vec2 Pos)
		:RLCollectible(typeID, Pos)
	{
	
	
	}
	void RLCollectibleLevelUpPerk::onCollect(RLHero * hero)
	{
		hero->applyEffect("GreenSkin");
		RLWorld::shared()->clearLevelUpPerk();
	}

	RLCollectibleMgr::RLCollectibleMgr()
	{
		m_collectibleSpriteType = RLSpritePool::shared()->get()->addTileType("RL/Bullet.png");
	}

	RLCollectible * RLCollectibleMgr::addCollectible(int typeID, vec2 pos)
	{
		RLCollectible * collectible = nullptr;
		switch(typeID)
		{
		case 0:
			collectible = new RLCollectible(typeID, pos);
			break;
		case 1:
			collectible = new RLCollectibleLevelUpPerk(typeID, pos);
			break;
		
		}

		if(collectible)
		{
			m_collectible.push_back(collectible);
			collectible->initGraphics();
		}
		return collectible;
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