#include "GameWorld.h"
#include "EngineSrc/3D/Terrain/GameMap.h"
#include "EngineSrc/3D/Terrain/GameMapConfig.h"
#include "EngineSrc/Event/EventMgr.h"
#include "Utility/log/Log.h"
#include "Utility/misc/Tmisc.h"
#include <algorithm>
#include "CropSystem.h"
#include "Collision/PhysicsMgr.h"
#include "BuildingSystem.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "AssistDrawSystem.h"
#include "GameItemMgr.h"
#include "PartSurfaceMgr.h"
#include "BulletMgr.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "Utility/file/Tfile.h"
#include "Engine/WorkerThreadSystem.h"
#include "Shader/ShaderMgr.h"
#include <filesystem>
#include "LoadingUI.h"
#include "EngineSrc/3D/Terrain/TerrainRuntime.h"
#include "EngineSrc/3D/Terrain/TerrainOctreeTypes.h"
#include "EngineSrc/3D/Terrain/TerrainEditSystem.h"
#include "Rail/RailSystem.h"

#include "Utility/file/JsonUtility.h"

#include <cstdio>
#include <system_error>

namespace tzw {

namespace
{
bool writeJsonFile(const std::filesystem::path& filePath, const rapidjson::Value& value)
{
	const std::string pathString = filePath.string();
	FILE* file = fopen(pathString.c_str(), "w");
	if (!file)
	{
		tlog("[error] failed to write json file %s", pathString.c_str());
		return false;
	}

	char writeBuffer[65536];
	rapidjson::FileWriteStream stream(file, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
	writer.SetIndent('\t', 1);
	const bool writeOk = value.Accept(writer) && ferror(file) == 0;
	const bool closeOk = fclose(file) == 0;
	if (!writeOk || !closeOk)
	{
		tlog("[error] failed to finish json file %s", pathString.c_str());
		return false;
	}
	return true;
}
}

bool WorldInfo::save(std::string filepath)
{
	rapidjson::Document doc;
	doc.SetObject();
	auto& aloc = doc.GetAllocator();
	doc.AddMember("WorldName", std::string(m_gameName), aloc);
	doc.AddMember("GameMode", m_gameMode, aloc);
	doc.AddMember("TerrainProceduralType", m_terrainProceduralType, aloc);
	doc.AddMember("ProceduralSeed", m_proceduralSeed, aloc);
	return writeJsonFile(filepath, doc);
}

void WorldInfo::load(std::string filepath)
{
	rapidjson::Document doc = Tfile::shared()->getJsonObject(filepath);
	std::string worldName = doc["WorldName"].GetString();
	strcpy(m_gameName, worldName.c_str());
	m_gameMode = doc["GameMode"].GetInt();
	m_terrainProceduralType = doc.HasMember("TerrainProceduralType") ? doc["TerrainProceduralType"].GetInt() : 1;
	m_proceduralSeed = doc.HasMember("ProceduralSeed") ? doc["ProceduralSeed"].GetInt() : 1337;
	tlog("world %s load finished, GameMode %d TerrainProceduralType %d ProceduralSeed %d", m_gameName, m_gameMode, m_terrainProceduralType, m_proceduralSeed);
}

GameWorld *GameWorld::m_instance = nullptr;
GameWorld *GameWorld::shared()
{
    if(!m_instance)
    {
        m_instance = new GameWorld();
    }
    return m_instance;
}

void GameWorld::createWorld(Scene *scene, int widthVoxels, int depthVoxels, int heightVoxels, float ratio)
{
    m_scene = scene;
    m_width = widthVoxels;
    m_depth = depthVoxels;
    m_height = heightVoxels;
	GameMapInitInfo mapInfo;
	mapInfo.ratio = ratio;
	mapInfo.widthVoxels = widthVoxels;
	mapInfo.depthVoxels = depthVoxels;
	mapInfo.heightVoxels = heightVoxels;
	mapInfo.blockSize = BLOCK_SIZE;
	mapInfo.proceduralSeed = m_currWorldInfo.m_proceduralSeed;
	GameMap::shared()->init(mapInfo);
	CropSystem::shared()->initFromFile();

	vec3 mapOffset = GameMap::shared()->getMapOffset();
	TerrainOctreeConfig config = TerrainOctreeConfig::fromVoxelDomain(
		GameMap::shared()->widthVoxels(), GameMap::shared()->heightVoxels(), GameMap::shared()->depthVoxels(),
		TERRAIN_MESH_CELL_COUNT, GameMap::shared()->blockSize(), mapOffset);
	m_terrainRuntime = std::make_unique<TerrainRuntime>();
	m_terrainRuntime->init(config);
	m_terrainRuntime->setDebugWireframeEnabled(false);
	m_railSystem = std::make_unique<RailSystem>();
	m_railSystem->init();
}

void GameWorld::createWorldFromFile(Scene* scene, int widthVoxels, int depthVoxels, int heightVoxels, float ratio, std::string filePath)
{
	(void)filePath;
	m_scene = scene;
	m_width = widthVoxels;
    m_depth = depthVoxels;
    m_height = heightVoxels;
	GameMapInitInfo mapInfo;
	mapInfo.ratio = ratio;
	mapInfo.widthVoxels = widthVoxels;
	mapInfo.depthVoxels = depthVoxels;
	mapInfo.heightVoxels = heightVoxels;
	mapInfo.blockSize = BLOCK_SIZE;
	mapInfo.proceduralSeed = m_currWorldInfo.m_proceduralSeed;
	GameMap::shared()->init(mapInfo);
	CropSystem::shared()->initFromFile();
	auto worldLocation = getWorldLocation();
	const std::filesystem::path terrainManifestPath = worldLocation / "Terrain.json";
	const std::filesystem::path terrainPagePath = worldLocation / "TerrainPages.bin";
	if (std::filesystem::exists(terrainManifestPath)
		&& !GameMap::shared()->unserializeTerrainSnapshot(terrainManifestPath.string(), terrainPagePath.string()))
	{
		tlog("[error] failed to load terrain snapshot %s", terrainManifestPath.string().c_str());
	}

	vec3 mapOffset = GameMap::shared()->getMapOffset();
	TerrainOctreeConfig config = TerrainOctreeConfig::fromVoxelDomain(
		GameMap::shared()->widthVoxels(), GameMap::shared()->heightVoxels(), GameMap::shared()->depthVoxels(),
		TERRAIN_MESH_CELL_COUNT, GameMap::shared()->blockSize(), mapOffset);
	m_terrainRuntime = std::make_unique<TerrainRuntime>();
	m_terrainRuntime->init(config);
	m_railSystem = std::make_unique<RailSystem>();
	m_railSystem->init();
	const std::filesystem::path railPath = worldLocation / "Rail.json";
	if (std::filesystem::exists(railPath))
	{
		auto railDoc = Tfile::shared()->getJsonObject(railPath.string());
		if (!m_railSystem->unserialize(railDoc))
		{
			tlog("[error] failed to load rail data %s", railPath.string().c_str());
			m_railSystem->clear();
			m_railSystem->init();
		}
	}
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

void GameWorld::onFrameUpdate(float delta)
{
	if (m_currentState != GAME_STATE_RUNNING)
		return;

	if (m_terrainRuntime && m_terrainRuntime->isActive())
	{
		vec3 playerPos = m_player->getPos();
		m_terrainRuntime->update(playerPos, m_mainRoot, delta);
	}
	if (m_railSystem)
	{
		m_railSystem->update(m_mainRoot, delta);
	}

	BuildingSystem::shared()->update(delta);
	AssistDrawSystem::shared()->handleDraw(delta);
	BulletMgr::shared()->handleDraw(delta);
}

void GameWorld::startGame(WorldInfo worldInfo)
{
	m_currWorldInfo = worldInfo;
	prepare();
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init Map",
		WorkerJob([&]()
	{
		auto player = new CubePlayer(m_mainRoot);
		GameWorld::shared()->setPlayer(player);
		//m_mainRoot->addChild(player);
			//init UI
		GameMap::shared()->setMapType(GameMap::MapType::Noise);
		GameMap::shared()->setMaxHeight(10);
		GameMap::shared()->setMinHeight(3);
		Tmisc::DurationBegin();
		createWorld(g_GetCurrScene(), GAME_MAP_WIDTH_VOXELS, GAME_MAP_DEPTH_VOXELS, GAME_MAP_HEIGHT_VOXELS, 0.05);
		tlog("init chunk cost : %d", Tmisc::DurationEnd());
		vec2 startPos = GameMap::shared()->getCenterOfMap();
		float height = GameMap::shared()->getHeight(startPos);
		m_player->setPos(vec3(startPos.x, height + 0.5, startPos.y));
		//m_player->setIsOpenJetPack(true);
	}));
	WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&]()
	{
		//call Script
		ScriptPyMgr::shared()->callFunPyVoid("tzw", "on_game_start");
		
		m_currentState = GAME_STATE_RUNNING;
	}));
	
}

void GameWorld::loadGame(std::string worldName)
{
	std::filesystem::path saveDir = "Data/PlayerData/Save/";
	;
	//load world Meta info
	m_currWorldInfo.load((saveDir/ worldName / "meta.json").string());
	prepare();
	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Init Map",
		WorkerJob([&]()
	{
		auto player = new CubePlayer(m_mainRoot);
		GameWorld::shared()->setPlayer(player);
		//m_mainRoot->addChild(player);
		//init UI
		GameMap::shared()->setMapType(GameMap::MapType::Noise);
		GameMap::shared()->setMaxHeight(10);
		GameMap::shared()->setMinHeight(3);
		createWorldFromFile(g_GetCurrScene(), GAME_MAP_WIDTH_VOXELS, GAME_MAP_DEPTH_VOXELS, GAME_MAP_HEIGHT_VOXELS, 0.05, "");
		loadPlayerInfo();
	}));

	WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([=]()
	{
		std::filesystem::path staticObjPath =  this->getWorldLocation() / "StaticObj.json";
		auto doc = Tfile::shared()->getJsonObject(staticObjPath.string());
		//load Static IslandList
		BuildingSystem::shared()->loadStatic(doc);
	}));
	WorkerThreadSystem::shared()->pushMainThreadOrder(WorkerJob([&]()
	{
		//call Script
		//ScriptPyMgr::shared()->callFunPyVoid("on_game_start");
		m_currentState = GAME_STATE_RUNNING;
	}));
}

void GameWorld::saveGame(std::string filePath)
{
	(void)filePath;
	std::filesystem::path worldLocation = getWorldLocation();

	tlog("world location is %s", worldLocation.string().c_str());
	std::error_code fsError;
	std::filesystem::create_directories(worldLocation, fsError);
	if (fsError)
	{
		tlog("[error] failed to create world directory %s: %s",
			worldLocation.string().c_str(), fsError.message().c_str());
		return;
	}

	//save world meta info
	{
		std::filesystem::path metaPath = worldLocation / "meta.json";
		if (!m_currWorldInfo.save(metaPath.string()))
		{
			return;
		}
	}
	
	//save static directory
	{
		std::filesystem::path staticObjPath = worldLocation / "StaticObj.json";
		rapidjson::Document doc;
		doc.SetObject();
		BuildingSystem::shared()->dumpStatic(doc, doc.GetAllocator());
		if (!writeJsonFile(staticObjPath, doc))
		{
			return;
		}
	}


	//save playerInfo
	if (!savePlayerInfo())
	{
		return;
	}

	//save terrain
	if (!GameMap::shared()->serializeTerrainSnapshot((worldLocation / "Terrain.json").string(),
		(worldLocation / "TerrainPages.bin").string()))
	{
		tlog("[error] failed to save terrain snapshot");
	}

	//save rail
	if (m_railSystem)
	{
		std::filesystem::path railPath = worldLocation / "Rail.json";
		rapidjson::Document doc;
		doc.SetObject();
		m_railSystem->serialize(doc, doc.GetAllocator());
		if (!writeJsonFile(railPath, doc))
		{
			return;
		}
	}
}

bool GameWorld::onKeyPress(int keyCode)
{
    switch (keyCode)
	{
    default:
        break;
    }
	return false;
}

bool GameWorld::onKeyRelease(int keyCode)
{
	switch (keyCode)
	{
	case TZW_KEY_F2:
		{
			ShaderMgr::shared()->reloadAllShaders();
		}
		break;
	case TZW_KEY_F3:
		{
			ScriptPyMgr::shared()->reload();
		}
		break;
	default: ;
	}
	return false;
}

void GameWorld::init()
{

	m_mainMenu = GameUISystem::shared();
	m_mainMenu->init();
	m_mainMenu->hide();
    m_mainRoot = new Node();
	g_GetCurrScene()->addNode(m_mainRoot);
	WorkerThreadSystem::shared()->setLoadingTipsSetter(
		[](const std::string& tips) { LoadingUI::shared()->setTipsInfo(tips); });
	ScriptPyMgr::shared()->callFunPyVoid("tzw","tzw_on_game_ready");
	
}

GameWorld::GameWorld()
{
    EventMgr::shared()->addStandaloneEventListener(this);
	setIsSwallow(false);
    m_currentState = GAME_STATE_SPLASH;
}

GameWorld::~GameWorld()
{
	tlog("hello world");
}

bool GameWorld::savePlayerInfo()
{
	std::filesystem::path worldLocation = getWorldLocation();
	rapidjson::Document doc;
	doc.SetObject();
	JsonUtility::shared()->addV3(doc, doc, "PlayerPos", getPlayer()->getPos());
	JsonUtility::shared()->addV4(doc, doc, "PlayerRot", getPlayer()->getRotateQ().toVec4());
	return writeJsonFile(worldLocation / "player.json", doc);
}

void GameWorld::loadPlayerInfo()
{
	std::filesystem::path worldLocation = getWorldLocation();
	auto filePath = worldLocation / "player.json";
	auto doc = Tfile::shared()->getJsonObject(filePath.string());
	m_player->setPos(JsonUtility::shared()->getV3(doc["PlayerPos"]));
	m_player->setRotateQ(JsonUtility::shared()->getV4(doc["PlayerRot"]));
}

std::filesystem::path GameWorld::getWorldLocation()
{
	std::filesystem::path prefix ="./Data/PlayerData/Save/";
	std::filesystem::path worldLocation = prefix;
	worldLocation.append(m_currWorldInfo.m_gameName);
	return worldLocation;
}

void GameWorld::prepare()
{


	WorkerThreadSystem::shared()->pushMainThreadOrderWithLoading("Load Blocks",
		WorkerJob([&]()
	{
		GameItemMgr::shared()->loadFromFile("Blocks/Blocks.json");
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
	if (m_terrainRuntime)
	{
		m_terrainRuntime->clear();
		m_terrainRuntime.reset();
	}
	if (m_railSystem)
	{
		m_railSystem->clear();
		m_railSystem.reset();
	}
    m_mainRoot->purgeAllChildren();
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

TerrainRuntime* GameWorld::terrainRuntime() const
{
	return m_terrainRuntime.get();
}

TerrainEditSystem* GameWorld::getTerrainEditSystem() const
{
	return m_terrainRuntime ? m_terrainRuntime->editSystem() : nullptr;
}

RailSystem* GameWorld::railSystem() const
{
	return m_railSystem.get();
}

} // namespace tzw
