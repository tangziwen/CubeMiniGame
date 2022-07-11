#include "RLBulletPool.h"

#include "2D/Sprite.h"


namespace tzw
{
RLBulletPool::RLBulletPool()
{
}
void RLBulletPool::initSpriteList(Node * node)
{
	m_spriteMgr = new SpriteInstanceMgr();
	m_spriteMgr->setLocalPiority(3);
	node->addChild(m_spriteMgr);
	int defaultBulletType = m_spriteMgr->addTileType("PTM/mountains.png");
	m_spriteTotalPool.resize(512);

	for(int i = 0; i < m_spriteTotalPool.size(); i++)
	{
		m_spriteTotalPool[i].type = defaultBulletType;
		m_spriteTotalPool[i].m_isVisible = false;
		m_freeSpriteList.push_back(&m_spriteTotalPool[i]);
		m_spriteMgr->addTile(&m_spriteTotalPool[i]);
	}
}

void RLBulletPool::spawnBullet(int type, vec2 pos, vec2 velocity)
{
	RLBullet newBullet;
	newBullet.m_pos = pos;
	newBullet.m_velocity = velocity;
	newBullet.m_sprite = assignASprite();
	newBullet.m_sprite->m_isVisible = true;
	m_bulletsList.emplace_back(newBullet);
}
void RLBulletPool::tick(float dt)
{

	for(auto iter = m_bulletsList.begin();iter != m_bulletsList.end();)
	{
		RLBullet & bullet = *iter;
		if(bullet.m_sprite)
		{
			bullet.m_pos += bullet.m_velocity * dt;
			bullet.m_sprite->pos = bullet.m_pos;
			bullet.m_t += dt;
			if(bullet.m_t > bullet.m_lifespan)
			{
				//ready to kill
				returnSprite(bullet.m_sprite);
				iter = m_bulletsList.erase(iter);
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

}
