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
#include "Base/TranslationMgr.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleInitVelocityModule.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "BulletMgr.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "Utility/file/Tfile.h"
#include "Engine/WorkerThreadSystem.h"
#include "LoadingUI.h"

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
	 
    
	 
    m_width = width;
	 
    m_depth = depth;
	 
    m_height = height;
	GameMap::shared()->init(ratio, m_width, m_depth, m_height);
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
    
	 
}

void GameWorld::createWorldFromFile(Scene* scene, int width, int depth, int height, float ratio, std::string filePath)
{
	m_scene = scene;
    m_width = width;
	 
    m_depth = depth;
	 
    m_height = height;
	GameMap::shared()->init(ratio, m_width, m_depth, m_height);
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
				chunk->initData();
                m_mainRoot->addChild(chunk);
				 
                m_chunkList.push_back(chunk);
				 
                m_chunkArray[i][j][k] = chunk;
				 
            }
        }
    }
	auto terrainFile = fopen(filePath.c_str(), "rb");
	int count;
	fread(&count,sizeof(int),1, terrainFile);
	for(int i = 0; i < count; i++)
	{
		int x, y, z;
		fread(&x, sizeof(int), 1, terrainFile);
		fread(&y, sizeof(int), 1, terrainFile);
		fread(&z, sizeof(int), 1, terrainFile);
		GameMap::shared()->getChunkInfo(x,y,z)->loadChunk(terrainFile);
		//auto chunk = m_chunkArray[x][y][z];
		//chunk->loadChunk(terrainFile);
	}
	fclose(terrainFile);
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
	BulletMgr::shared()->handleDraw(delta);
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
	prepare();
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init Map",
		WorkerJob([&]()
	{
		auto player = new CubePlayer(m_mainRoot);
		GameWorld::shared()->setPlayer(player);
		m_mainRoot->addChild(player);
			//init UI
		GameMap::shared()->setMapType(GameMap::MapType::Noise);
		GameMap::shared()->setMaxHeight(10);
		GameMap::shared()->setMinHeight(3);
		createWorld(g_GetCurrScene(),GAME_MAP_WIDTH, GAME_MAP_DEPTH, GAME_MAP_HEIGHT, 0.05);
		
		float height = GameMap::shared()->getDensity(vec3(0, 0, 0));
		m_player->setPos(vec3(0, height + 3, 0));
		loadChunksAroundPlayer();
	}));
	WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&]()
	{
		//call Script
		ScriptPyMgr::shared()->callFunV("on_game_start");
		
		m_currentState = GAME_STATE_RUNNING;
	}));
	
}

void GameWorld::loadGame(std::string filePath)
{
	prepare();
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init Map",
		WorkerJob([&]()
	{
		auto player = new CubePlayer(m_mainRoot);
		GameWorld::shared()->setPlayer(player);
		m_mainRoot->addChild(player);
		//init UI
		GameMap::shared()->setMapType(GameMap::MapType::Noise);
		GameMap::shared()->setMaxHeight(10);
		 
		GameMap::shared()->setMinHeight(3);
		createWorldFromFile(g_GetCurrScene(),GAME_MAP_WIDTH, GAME_MAP_DEPTH, GAME_MAP_HEIGHT, 0.05, "./Terrain.bin");
		m_player->setPos(vec3(5, 20.0, -5));
		loadChunksAroundPlayer();
	}));

	WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([=]()
	{
		
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}

		//load Static IslandList
		BuildingSystem::shared()->loadStatic(doc);
	}));
	WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&]()
	{
		//call Script
		ScriptPyMgr::shared()->callFunV("on_game_start");
		m_currentState = GAME_STATE_RUNNING;
	}));
}

void GameWorld::saveGame(std::string filePath)
{
	rapidjson::Document doc;
	doc.SetObject();
	BuildingSystem::shared()->dumpStatic(doc, doc.GetAllocator());

	rapidjson::StringBuffer buffer;
	auto file = fopen(filePath.c_str(), "w");
	char writeBuffer[65536];
	rapidjson::FileWriteStream stream(file, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
	writer.SetIndent('\t', 1);
	doc.Accept(writer);
	fclose(file);


	std::vector<ChunkInfo * > tmpChunkList;
	tmpChunkList.reserve(2048);
	
    for(int i = 0;i< m_width;i++)
    {
        for(int j=0;j<m_height;j++)
        {
            for(int k = 0; k < m_depth; k++)
            {	 
                auto chunkInfo = GameMap::shared()->getChunkInfo(i, j, k);
            	if(chunkInfo->isLoaded)
            	{
            		tmpChunkList.push_back(chunkInfo);
            	}
            }
        }
    }
    auto terrainFile = fopen("./Terrain.bin", "wb");
	//first size of int tell the fucking count
	int theSize = tmpChunkList.size();
	fwrite(&theSize, sizeof(int),1 ,terrainFile);
	for(auto chunk : tmpChunkList)
	{
		chunk->dumpChunk(terrainFile);
	}
    fclose(terrainFile);
	
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

	m_mainMenu = GameUISystem::shared();
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
    m_currentState = GAME_STATE_SPLASH;
}

GameWorld::~GameWorld()
{
	tlog("hello world");
}

void GameWorld::prepare()
{
	
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Load Blocks",
		WorkerJob([&]()
	{
		ItemMgr::shared()->loadFromFile("./Res/Blocks/Blocks.json");
	}));
	//load the BlockConfig

	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init Physics",WorkerJob(
		[&]()
	{
		PhysicsMgr::shared()->start();
	}));
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init Data",
		WorkerJob([&]()
	{
		unloadGame();
	}));
	Tmisc::DurationBegin();
		//load Surface
	PartSurfaceMgr::shared()->loadFromFile("Some_surface_not_used_yet");
	
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init UI",
		WorkerJob([&]()
	{
		//init UI
		GameUISystem::shared()->initInGame();
	}));
}

GameUISystem *GameWorld::getMainMenu() const
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

