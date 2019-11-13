#include "GameWorld.h"
#include "GameMap.h"
#include "EngineSrc/Event/EventMgr.h"
#include "Utility/log/Log.h"
#include "time.h"
#include "Utility/misc/Tmisc.h"
#include <algorithm>
#include "GameConfig.h"
#include <thread>
#include "Collision/PhysicsMgr.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "BuildingSystem.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "AssistDrawSystem.h"
#include "ItemMgr.h"
#include "PartSurfaceMgr.h"

namespace tzw {
GameWorld *GameWorld::m_instance = nullptr;
GameWorld *GameWorld::shared()
{
    if(!m_instance)
    {
        m_instance = new GameWorld();
    }
    return m_instance;
}

void GameWorld::createWorld(Scene *scene, int width, int depth, int height, float ratio)
{
	 
    m_scene = scene;
	 
    GameMap::shared()->init(ratio);
	 
    m_width = width;
	 
    m_depth = depth;
	 
    m_height = height;
	 
	float offsetX = -1 * width * MAX_BLOCK * BLOCK_SIZE / 2;
	 
	float offsetZ =  depth * MAX_BLOCK * BLOCK_SIZE / 2; // notice the signed!
	 
	m_mapOffset = vec3(offsetX, 0 ,offsetZ);
	 
    for(int i = 0;i< m_width;i++)
    {
        for(int j=0;j<m_height;j++)
        {
            for(int k = 0; k < m_depth; k++)
            {
                auto chunk = new Chunk(i,j,k);
				 
                m_mainRoot->addChild(chunk);
				 
                m_chunkList.push_back(chunk);
				 
                m_chunkArray[i][j][k] = chunk;
				 
            }
        }
    }
	 
    loadChunksAroundPlayer();
	 
}

vec3 GameWorld::worldToGrid(vec3 world)
{
    return vec3((world.x+1) / BLOCK_SIZE, (world.y+1) / BLOCK_SIZE, (-world.z+1) / BLOCK_SIZE);
}

vec3 GameWorld::gridToChunk(vec3 grid)
{
    return vec3(grid.x / MAX_BLOCK, grid.y / MAX_BLOCK, grid.z / MAX_BLOCK);
}

CubePlayer *GameWorld::getPlayer() const
{
    return m_player;
}

void GameWorld::setPlayer(CubePlayer *player)
{
    m_player = player;
}

Chunk *GameWorld::getOrCreateChunk(int x, int y, int z)
{
    auto chunk = getChunk(x,y,z);
    if(!chunk)
    {
        chunk = createChunk(x,y,z);
    }
    return chunk;
}

void GameWorld::onFrameUpdate(float delta)
{
	if (m_currentState != GAME_STATE_RUNNING)
		return;
	BuildingSystem::shared()->update(delta);
	AssistDrawSystem::shared()->handleDraw(delta);
}

Chunk *GameWorld::createChunk(int x, int y, int z)
{
    auto chunkA = new Chunk(x,0,z);
    m_scene->addNode(chunkA);
    m_chunkList.push_back(chunkA);
    return chunkA;
}

void GameWorld::startGame()
{
	//load the config
	ItemMgr::shared();
	m_currentState = GAME_STATE_RUNNING;
	PhysicsMgr::shared()->start();
	Tmisc::DurationBegin();
	 
	unloadGame();
	PartSurfaceMgr::shared()->loadFromFile("aaaaaa");
	MainMenu::shared()->initInGame();
	GameMap::shared()->setMapType(GameMap::MapType::Plain);
	GameMap::shared()->setMaxHeight(10);
	 
	GameMap::shared()->setMinHeight(3);
	auto player = new CubePlayer(m_mainRoot);
	 
	GameWorld::shared()->setPlayer(player);
	 
	GameWorld::shared()->createWorld(g_GetCurrScene(),GAME_MAP_WIDTH, GAME_MAP_DEPTH, GAME_MAP_HEIGHT, 0.05);
	 
	m_mainRoot->addChild(player);

	ScriptPyMgr::shared()->callFunV("on_game_start");
	
	//PhysicsMgr::shared()->start();
	//PhysicsMgr::shared()->createPlane(0,1,0, 10);
}

bool GameWorld::onKeyPress(int keyCode)
{
    switch (keyCode)
	{
    default:
        break;
    }
	return true;
}
void GameWorld::loadChunksAroundPlayer()
{
	 
	Tmisc::DurationBegin();
	 
    std::set<Chunk*> m_tempArray = m_activedChunkList;
	 
	std::vector<Chunk *> m_readyToLoadArray;
	 
    auto pos = m_player->getPos() - m_mapOffset;
	 
    int posX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
	 
    int posZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
	int range = ceil(100.0f / (MAX_BLOCK * BLOCK_SIZE));
	 
    for(int i =posX - range;i<=posX + range;i++)
    {
        for(int j =0;j< m_height; j++)
        {
            for( int k = posZ - range; k <= posZ + range; k++)
            {
                if(i < 0 || i >= m_width || j < 0 || j >= m_height || k < 0 || k >= m_depth)
                    continue;
				 
                auto targetChunk = m_chunkArray[i][j][k];
				 
				m_readyToLoadArray.push_back(targetChunk);
				 
                auto findResult = m_tempArray.find(targetChunk);
				 
                if(findResult!= m_tempArray.end())
                {
                    m_tempArray.erase(findResult);
                }else
                {
                    m_activedChunkList.insert(m_chunkArray[i][j][k]);
                }
            }
        }
    }
    for(Chunk* i:m_tempArray)
    {
		 
        i->unload();
		 
    }
	std::sort(m_readyToLoadArray.begin(), m_readyToLoadArray.end(),[&](Chunk * left, Chunk * right)
	{
		float distl = left->getPos().distance(m_player->getPos());
		float distr = right->getPos().distance(m_player->getPos());
		return distl < distr;
	}
	);
	for(Chunk * i :m_readyToLoadArray)
	{
		 
		i->load();
		 
	}
	tlog("load chunk cost : %d", Tmisc::DurationEnd());
}

tzw::vec3 GameWorld::getMapOffset() const
{
	return m_mapOffset;
}

void GameWorld::setMapOffset(tzw::vec3 val)
{
	m_mapOffset = val;
}

void GameWorld::init()
{
	m_mainMenu = MainMenu::shared();
	m_mainMenu->init();
	m_mainMenu->hide();
    m_mainRoot = new Node();
	g_GetCurrScene()->addNode(m_mainRoot);
	ScriptPyMgr::shared()->callFunV("tzw_on_game_ready");
}

Chunk *GameWorld::getChunk(int x, int y, int z)
{
    if(x >=0 && x <m_width && z >=0 && z <m_depth && y >=0 && y < m_height)
    {
        return m_chunkArray[x][y][z];
    }
    else
    {
        return nullptr;
    }
}

GameWorld::GameWorld()
{
    EventMgr::shared()->addFixedPiorityListener(this);
    memset(m_chunkArray, 0, 128 * 128 * 16 * sizeof(Chunk *));
    m_currentState = GAME_STATE_MAIN_MENU;
}

GameWorld::~GameWorld()
{
	tlog("hello world");
}

MainMenu *GameWorld::getMainMenu() const
{
    return m_mainMenu;
}

void GameWorld::unloadGame()
{
    m_mainRoot->purgeAllChildren();
    m_activedChunkList.clear();
    m_chunkList.clear();
	memset(m_chunkArray, 0, 128 * 128 * 16 * sizeof(Chunk *));
}

int GameWorld::getCurrentState() const
{
    return m_currentState;
}

void GameWorld::setCurrentState(const int &currentState)
{
    m_currentState = currentState;
}

Node *GameWorld::getMainRoot() const
{
    return m_mainRoot;
}

void GameWorld::setMainRoot(Node *mainRoot)
{
    m_mainRoot = mainRoot;
}

} // namespace tzw

