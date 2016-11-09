#include "GameWorld.h"
#include "GameMap.h"
#include "EngineSrc/Event/EventMgr.h"
#include "TUtility/TUtility.h"
#include "time.h"
#include "EngineSrc/3D/SkyBox.h"
#include <iostream>
#include "GameConfig.h"
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
    Tmisc::DurationBegin();
    for(int i = 0;i< width;i++)
    {
        for(int j=0;j<m_height;j++)
        {
            for(int k = 0; k < m_depth; k++)
            {
                auto chunkA = new Chunk(i,j,k);
                m_mainRoot->addChild(chunkA);
                m_chunkList.push_back(chunkA);
                m_chunkArray[i][j][k] = chunkA;
				chunkA->load();
            }
        }
    }
    //loadChunksAroundPlayer();
    std::cout <<"load world need" << Tmisc::DurationEnd() << std::endl;
}

vec3 GameWorld::worldToGrid(vec3 world)
{
    return vec3((world.x+1) / BLOCK_SIZE, (world.y+1) / BLOCK_SIZE, (-world.z+1) / BLOCK_SIZE);
}

vec3 GameWorld::gridToChunk(vec3 grid)
{
    return vec3(grid.x / MAX_BLOCK, grid.y / MAX_BLOCK, grid.z / MAX_BLOCK);
}

Player *GameWorld::getPlayer() const
{
    return m_player;
}

void GameWorld::setPlayer(Player *player)
{
    m_player = player;
}

Chunk *GameWorld::getOrCreateChunk(int x, int y, int z)
{
    auto chunk = getChunkByChunk(x,y,z);
    if(!chunk)
    {
        chunk = createChunk(x,y,z);
    }
    return chunk;
}

void GameWorld::onFrameUpdate(float delta)
{
    if(m_currentState != GameState::OnPlay)
        return;
    static float timer = 0;
    timer += delta;
    if (timer >= 5.0f)
    {
        //loadChunksAroundPlayer();
        timer = 0.0f;
    }
}

Chunk *GameWorld::createChunk(int x, int y, int z)
{
    auto chunkA = new Chunk(x,0,z);
    m_scene->addNode(chunkA);
    m_chunkList.push_back(chunkA);
    return chunkA;
}

void GameWorld::loadBlockSheet()
{
    m_blockSheet = new TextureAtlas("./Res/User/CubeGame/texture/dest/blocks.json");
    m_blockSheet->texture()->setFilter(Texture::FilterType::Nearest);
}

TextureAtlas *GameWorld::getBlockSheet() const
{
    return m_blockSheet;
}

void GameWorld::setBlockSheet(TextureAtlas *blockSheet)
{
    m_blockSheet = blockSheet;
}

void GameWorld::startGame()
{
    unloadGame();
    GameWorld::shared()->loadBlockSheet();
    crossHair = Sprite::create("./Res/User/CubeGame/texture/GUI/cross_hair.png");
    auto size = crossHair->getContentSize();
    crossHair->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
    m_mainRoot->addChild(crossHair);
    GameMap::shared()->setMapType(GameMap::MapType::Plain);
    GameMap::shared()->setMaxHeight(20);
    auto player = new Player(m_mainRoot);
    GameWorld::shared()->setPlayer(player);
    GameWorld::shared()->createWorld(g_GetCurrScene(),GAME_MAP_WIDTH, GAME_MAP_DEPTH, GAME_MAP_HEIGHT, 0.02);
    m_mainRoot->addChild(player);
    SkyBox* skybox = SkyBox::create("./Res/User/CubeGame/texture/SkyBox/right.jpg","./Res/User/CubeGame/texture/SkyBox/left.jpg",
                                "./Res/User/CubeGame/texture/SkyBox/top.jpg","./Res/User/CubeGame/texture/SkyBox/bottom.jpg",
                                "./Res/User/CubeGame/texture/SkyBox/back.jpg","./Res/User/CubeGame/texture/SkyBox/front.jpg");
    skybox->setScale(80,80,80);
    g_GetCurrScene()->setSkyBox(skybox);
    m_currentState = GameState::OnPlay;
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
    std::set<Chunk*> m_tempArray = m_activedChunkList;
    auto pos = m_player->getPos();
    int posX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
    int posZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
    for(int i =posX - 2;i<=posX + 2;i++)
    {
        for(int j =0;j< m_height; j++)
        {
            for( int k = posZ - 2; k <= posZ + 2; k++)
            {
                if(i < 0 || i >= m_width || j < 0 || j >= m_height || k < 0 || k >= m_depth)
                    continue;
                auto targetChunk = m_chunkArray[i][j][k];
                targetChunk->load();
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
}

Chunk *GameWorld::getChunkByChunk(int x, int y, int z)
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
    memset(m_chunkArray,CUBE_MAP_SIZE * CUBE_MAP_SIZE * GAME_MAP_HEIGHT * sizeof(Chunk *),0);
    m_currentState = GameState::MainMenu;
    m_mainMenu = new MainMenu();
    g_GetCurrScene()->addNode(m_mainMenu);
    m_mainRoot = new Node();
    g_GetCurrScene()->addNode(m_mainRoot);
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
    memset(m_chunkArray,CUBE_MAP_SIZE * CUBE_MAP_SIZE * GAME_MAP_HEIGHT * sizeof(Chunk *),0);
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

