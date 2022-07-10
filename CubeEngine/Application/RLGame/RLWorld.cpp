#include "RLWorld.h"
#include "RLHero.h"
#include "Event/EventMgr.h"
#include "RLBulletPool.h"
namespace tzw
{
void RLWorld::start()
{
	EventMgr::shared()->addFixedPiorityListener(this);

	m_tileMgr = new TileMap2DMgr();
	m_tileMgr->initMap(ARENA_MAP_SIZE, ARENA_MAP_SIZE);

	int tileTypeForest = m_tileMgr->addTileType("PTM/forest.png");
	int tileTypePlain = m_tileMgr->addTileType("PTM/plain.png");
	int tileTypeMountains = m_tileMgr->addTileType("PTM/mountains.png");
	int tileTypeWater = m_tileMgr->addTileType("PTM/water.png");
	for(int i = 0; i < ARENA_MAP_SIZE; i++)
	{
		for(int j = 0; j < ARENA_MAP_SIZE; j++)
		{
			m_tileMgr->addTile(tileTypeForest, i, j);
		}
	}
	m_mapRootNode = Node::create();
	g_GetCurrScene()->addNode(m_mapRootNode);
	m_mapRootNode->addChild(m_tileMgr);

	RLBulletPool::shared()->initSpriteList(m_mapRootNode);
	m_playerController = new RLPlayerController();

	RLHero * hero = spawnHero(0);
	hero->setPosition(vec2(50, 50));
	hero->equipWeapon(new RLWeapon());
	m_playerController->possess(hero);


}

Node* RLWorld::getRootNode()
{
	return m_mapRootNode;
}

void RLWorld::onFrameUpdate(float dt)
{
	for(RLHero * hero : m_heroes)
	{
		hero->onTick(dt);
	}
	RLBulletPool::shared()->tick(dt);
}

RLHero* RLWorld::spawnHero(int heroType)
{
	RLHero * hero = new RLHero(heroType);
	m_heroes.push_back(hero);
	return hero;
}

}
