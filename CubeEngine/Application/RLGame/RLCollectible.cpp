#include "RLCollectible.h"
#include "RLSpritePool.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLPlayerState.h"
#include "RLEffectMgr.h"
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
		m_collider->m_cb = nullptr;
		RLWorld::shared()->getQuadTree()->removeCollider(m_collider);
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

	void RLCollectible::onCollision(Collider2D* self, Collider2D* other)
	{
		
		if(other->getUserData().m_userData)
		{
			
			switch(other->getUserData().m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				RLHero * hero = reinterpret_cast<RLHero *>(other->getUserData().m_userData);
				if(m_isAlive && hero->getIsPlayerControll())
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
		RLPlayerState::shared()->gainExp(25);
		//hero->setPropByName<float>("MAXHP", 500);
	}

	int RLCollectible::getSpriteType()
	{
		return RLSpritePool::shared()->get()->getOrAddType("RL/Sprites/Exp.png");
	}

	RLCollectibleEXP::RLCollectibleEXP(unsigned int typeID, vec2 Pos)
		:RLCollectible(typeID, Pos)
	{
	
	
	}
	void RLCollectibleEXP::onCollect(RLHero * hero)
	{
		RLPlayerState::shared()->gainExp(25);
	}


	RLCollectibleGold::RLCollectibleGold(unsigned int typeID, vec2 Pos)
		:RLCollectible(typeID, Pos)
	{
	
	
	}
	void RLCollectibleGold::onCollect(RLHero * hero)
	{
		RLPlayerState::shared()->addGold(1);
	}

	int RLCollectibleGold::getSpriteType()
	{
		return RLSpritePool::shared()->get()->getOrAddType("RL/Sprites/Gold.png");
	}


	RLCollectibleLevelUpPerk::RLCollectibleLevelUpPerk(unsigned int typeID, vec2 Pos)
		:RLCollectible(typeID, Pos)
	{
	
	
	}
	void RLCollectibleLevelUpPerk::onCollect(RLHero * hero)
	{
		hero->applyEffect(m_effect);
		RLWorld::shared()->clearLevelUpPerk();
	}

	int RLCollectibleLevelUpPerk::getSpriteType()
	{
		return RLSpritePool::shared()->get()->getOrAddType(m_effect->getSpritePath());
	}

	void RLCollectibleLevelUpPerk::setEffect(RLEffect* effect)
	{
		m_effect = effect;
	}

	RLCollectibleMgr::RLCollectibleMgr()
	{
		m_collectibleSpriteType = RLSpritePool::shared()->get()->addTileType("RL/Sprites/Exp.png");
	}

	RLCollectible * RLCollectibleMgr::addCollectible(int typeID, vec2 pos)
	{
		RLCollectible * collectible = nullptr;
		switch(typeID)
		{
		case 0:
			collectible = new RLCollectibleEXP(typeID, pos);
			break;
		case 1:
			collectible = new RLCollectibleLevelUpPerk(typeID, pos);
			break;
		case 2:
			collectible = new RLCollectibleGold(typeID, pos);
			break;
		default:
			break;
		}

		if(collectible)
		{
			m_collectible.push_back(collectible);
			//collectible->initGraphics();
			SpriteInstanceInfo * sprite = RLCollectibleMgr::shared()->giveGraphics(collectible->getSpriteType());
			sprite->pos = pos;
			collectible->setSprite(sprite);
		}
		return collectible;
	}

	RLCollectible* RLCollectibleMgr::addCollectiblePerkEffect(RLEffect* effect, vec2 pos)
	{
		RLCollectibleLevelUpPerk * collectible = nullptr;
		collectible = new RLCollectibleLevelUpPerk(1, pos);
		collectible->setEffect(effect);
		if(collectible)
		{
			m_collectible.push_back(collectible);
			//collectible->initGraphics();
			SpriteInstanceInfo * sprite = RLCollectibleMgr::shared()->giveGraphics(collectible->getSpriteType());
			sprite->pos = pos;
			collectible->setSprite(sprite);
		}
		return collectible;
	}

	SpriteInstanceInfo* RLCollectibleMgr::giveGraphics(int collectibleType)
	{
		SpriteInstanceInfo * info = new SpriteInstanceInfo();
		info->type = collectibleType;
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

				
				iter = m_collectible.erase(iter);
				delete collectible;
			}
			else
			{

				collectible->getSprite()->pos = collectible->m_collider->getPos(); 
				++iter;
			}
		}
	}
}