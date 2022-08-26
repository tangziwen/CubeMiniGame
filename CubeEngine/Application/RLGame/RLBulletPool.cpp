#include "RLBulletPool.h"

#include "2D/Sprite.h"

#include "RLWorld.h"
namespace tzw
{
RLBulletPool::RLBulletPool()
{
}
int defaultBulletType = 0;
int EnemyBulletType = 0;

void RLBulletPool::initSpriteList(Node * node)
{
	m_spriteMgr = new SpriteInstanceMgr();
	m_spriteMgr->setLocalPiority(3);
	node->addChild(m_spriteMgr);
	defaultBulletType = m_spriteMgr->addTileType("RL/Bullet.png");
	EnemyBulletType = m_spriteMgr->addTileType("RL/EnemyBullet.png");
	m_spriteTotalPool.resize(1024);

	for(int i = 0; i < m_spriteTotalPool.size(); i++)
	{
		m_spriteTotalPool[i].type = defaultBulletType;
		m_spriteTotalPool[i].m_isVisible = false;
		m_freeSpriteList.push_back(&m_spriteTotalPool[i]);
		m_spriteMgr->addTile(&m_spriteTotalPool[i]);
	}
}

void RLBulletPool::spawnBullet(int type, vec2 pos, vec2 velocity, bool isAllyBullet, const RLBulletInfo & info)
{
	RLBullet * newBullet = new RLBullet();
	newBullet->m_pos = pos;
	newBullet->m_velocity = velocity;
	newBullet->m_sprite = assignASprite();
	newBullet->m_sprite->m_isVisible = true;
	newBullet->m_lifespan = 5.f;
	newBullet->m_info = info;
	newBullet->m_collider2D.setPos(pos);
	newBullet->m_collider2D.setRadius(16);
	newBullet->m_collider2D.setSourceChannel(CollisionChannel2D_Bullet);
	if(isAllyBullet)
	{
		newBullet->m_sprite->type = defaultBulletType;
		newBullet->m_collider2D.setResponseChannel(CollisionChannel2D_Entity);
	}
	else
	{
		newBullet->m_sprite->type = EnemyBulletType;
		newBullet->m_collider2D.setResponseChannel(CollisionChannel2D_Player);
	}
	
	m_bulletsList.push_back(newBullet);
	RLWorld::shared()->getQuadTree()->addCollider(&(newBullet->m_collider2D));
}
void RLBulletPool::tick(float dt)
{

	for(auto iter = m_bulletsList.begin();iter != m_bulletsList.end();)
	{
		RLBullet * bullet = *iter;
		if(bullet->m_sprite)
		{
			bullet->m_pos = bullet->m_collider2D.getPos();
			bullet->m_pos += bullet->m_velocity * dt;
			bullet->m_sprite->pos = bullet->m_pos;
			bullet->m_collider2D.setPos(bullet->m_pos);
			//RLWorld::shared()->getQuadTree()->checkCollision(&bullet->m_collider2D);
			bullet->m_t += dt;
			if((bullet->m_t > bullet->m_lifespan) || (!bullet->m_isLiving) || checkOutOfRange(bullet))
			{
				//ready to kill
				returnSprite(bullet->m_sprite);
				RLWorld::shared()->getQuadTree()->removeCollider(&bullet->m_collider2D);
				iter = m_bulletsList.erase(iter);
				delete bullet;
			}
			else
			{
				++iter;
			}
		}
	}
}

SpriteInstanceInfo* RLBulletPool::assignASprite()
{
	SpriteInstanceInfo * info = m_freeSpriteList.front();
	m_freeSpriteList.pop_front();
	return info;
}

void RLBulletPool::returnSprite(SpriteInstanceInfo* spriteInfo)
{
	spriteInfo->m_isVisible = false;
	m_freeSpriteList.push_back(spriteInfo);
}

bool RLBulletPool::checkOutOfRange(RLBullet* bullet)
{
	vec2 mapSize = RLWorld::shared()->getMapSize();
	return (bullet->m_pos.x < 0 || bullet->m_pos.x > mapSize.x || bullet->m_pos.y < 0 || bullet->m_pos.y > mapSize.y);
}

}
