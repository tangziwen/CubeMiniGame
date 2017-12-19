#include "GameWorld.h"
#include "GameMap.h"
#include "EngineSrc/Event/EventMgr.h"
#include "TUtility/TUtility.h"
#include "time.h"
#include "EngineSrc/3D/SkyBox.h"
#include "EngineSrc/3D/Sky.h"
#include <iostream>
#include "GameConfig.h"
#include <thread>
#include "Collision/PhysicsMgr.h"

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
	if (m_currentState != GameState::OnPlay)
		return;
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
	Tmisc::DurationBegin();
    unloadGame();
    crossHair = Sprite::create("./Res/User/CubeGame/texture/GUI/cross_hair.png");
    auto size = crossHair->getContentSize();
    crossHair->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
    m_mainRoot->addChild(crossHair);
    GameMap::shared()->setMapType(GameMap::MapType::Noise);
    GameMap::shared()->setMaxHeight(10);
	GameMap::shared()->setMinHeight(3);
    auto player = new CubePlayer(m_mainRoot);
    GameWorld::shared()->setPlayer(player);
    GameWorld::shared()->createWorld(g_GetCurrScene(),GAME_MAP_WIDTH, GAME_MAP_DEPTH, GAME_MAP_HEIGHT, 0.05);
    m_mainRoot->addChild(player);
    m_currentState = GameState::OnPlay;

	PhysicsMgr::shared()->start();
	PhysicsMgr::shared()->createPlane(0,1,0, 10);
}

bool tzw::GameWorld::onKeyPress(int keyCode)
{
    switch (keyCode) {
    case GLFW_KEY_ESCAPE:
        this->toggleMainMenu();
        break;
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
	int range = ceil(150.0f / (MAX_BLOCK * BLOCK_SIZE));
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
	std::cout << "load chunk cost :" << Tmisc::DurationEnd() << std::endl;
}

tzw::vec3 GameWorld::getMapOffset() const
{
	return m_mapOffset;
}

void GameWorld::setMapOffset(tzw::vec3 val)
{
	m_mapOffset = val;
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
    m_currentState = GameState::MainMenu;
    m_mainMenu = new MainMenu();
    g_GetCurrScene()->addNode(m_mainMenu);
    m_mainRoot = new Node();
	g_GetCurrScene()->addNode(m_mainRoot);

}

GameWorld::~GameWorld()
{
	printf("hello world");
}

MainMenu *GameWorld::getMainMenu() const
{
    return m_mainMenu;
}

void GameWorld::toggleMainMenu()
{
    m_mainMenu->toggle();
}

void GameWorld::unloadGame()
{
    m_mainRoot->purgeAllChildren();
    m_activedChunkList.clear();
    m_chunkList.clear();
	memset(m_chunkArray, 0, 128 * 128 * 16 * sizeof(Chunk *));
}

GameWorld::GameState GameWorld::getCurrentState() const
{
    return m_currentState;
}

void GameWorld::setCurrentState(const GameState &currentState)
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

