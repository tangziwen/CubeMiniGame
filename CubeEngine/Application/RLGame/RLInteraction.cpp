#include "RLInteraction.h"
#include "RLSpritePool.h"
#include "RLWorld.h"
namespace tzw
{



RLInteraction::RLInteraction(vec2 pos):m_pos(pos)
{
}

RLInteraction::~RLInteraction()
{
	RLSpritePool::shared()->get()->removeSprite(m_sprite);
}

void RLInteraction::use()
{
}

void RLInteraction::tick(float dt)
{
	m_isInteractiveable = false;
	vec2 pos = RLWorld::shared()->getPlayerController()->getPos();
	if((m_pos- pos).length() < 64.f)
	{
		m_isInteractiveable = true;
	}
}

void RLInteraction::initGraphics()
{
	int spriteType = RLSpritePool::shared()->get()->getOrAddType("RL/Sprites/Hatch.png");
	SpriteInstanceInfo * info = new SpriteInstanceInfo();
	info->type = spriteType;
	m_sprite = info;
	m_sprite->layer = 0;
	m_sprite->pos = m_pos;
	RLSpritePool::shared()->get()->addTile(m_sprite);
}

bool RLInteraction::isInteractiveable()
{
	return m_isInteractiveable;
}

void RLStair::use()
{
	RLWorld::shared()->startNextSubWave();
}

}