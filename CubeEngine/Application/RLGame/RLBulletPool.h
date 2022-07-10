#pragma once
#include <list>
#include <vector>

#include "Base/Singleton.h"
#include "Math/vec2.h"
#include "RLBullet.h"
#include "2D/SpriteInstanceMgr.h"
namespace tzw
{
class SpriteInstanceMgr;
struct SpriteInstanceInfo;
class RLBulletPool: public Singleton<RLBulletPool>
{
public:
	RLBulletPool();
	void initSpriteList(Node * node);
	void spawnBullet(int type, vec2 pos, vec2 velocity);
	void tick(float dt);
	SpriteInstanceInfo * assignASprite();
private:
	std::vector<RLBullet> m_bulletsList;
	std::list<SpriteInstanceInfo *> m_freeSpriteList;
	SpriteInstanceMgr * m_spriteMgr;
	std::vector<SpriteInstanceInfo> m_spriteTotalPool;
};
}
