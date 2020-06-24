#ifndef TZW_CHUNKMGR_H
#define TZW_CHUNKMGR_H

#include <filesystem>

#include "EngineSrc/CubeEngine.h"
#include "./Chunk.h"
#include "CubePlayer.h"
#include "GameUISystem.h"
#include "GameConfig.h"
#include <set>
namespace tzw {

#define GAME_STATE_MAIN_MENU 0
#define GAME_STATE_RUNNING 1
#define GAME_STATE_SPLASH 2


struct WorldInfo
{
    int m_gameMode;
	char m_gameName[64];
	int m_terrainProceduralType;
	int m_proceduralSeed;
	void save(std::string filepath);
	void load(std::string filepath);
};

class GameWorld : public EventListener
{
public:
    static GameWorld * shared();
    void createWorld(Scene* scene, int blockWitdh, int depth, int height, float ratio);
	void createWorldFromFile(Scene* scene, int blockWitdh, int depth, int height, float ratio, std::string filePath);
    vec3 worldToGrid(vec3 world);
    vec3 gridToChunk(vec3 grid);
    Chunk * getChunk(int x,int y,int z);
    CubePlayer *getPlayer() const;
    void setPlayer(CubePlayer *player);
    Chunk * getOrCreateChunk(int x,int y, int z);
	void onFrameUpdate(float delta) override;
    Chunk * createChunk(int x,int y,int z);
    void startGame(WorldInfo worldInfo);
	void loadGame(std::string filePath);
	void saveGame(std::string filePath);
	bool onKeyPress(int keyCode) override;
	bool onKeyRelease(int keyCode) override;
    GameUISystem *getMainMenu() const;
    void unloadGame();
    int getCurrentState() const;
    void setCurrentState(const int &currentState);
    Node *getMainRoot() const;
    void setMainRoot(Node *mainRoot);
	void loadChunksAroundPlayer();
	void init();
	virtual ~GameWorld();
	void savePlayerInfo();
	void loadPlayerInfo();
private:
	std::filesystem::path getWorldLocation();
    Node * m_mainRoot;
    int m_currentState;
    int m_width, m_depth, m_height;
    Chunk * m_chunkArray[CUBE_MAP_SIZE][16][CUBE_MAP_SIZE];
    Scene  * m_scene;
    CubePlayer * m_player;
    std::vector<Chunk*> m_chunkList;
    static GameWorld *m_instance;
    GameWorld();
    std::set<Chunk*> m_activedChunkList;
    GameUISystem * m_mainMenu;
	void prepare();
	void initChunk();
	WorldInfo m_currWorldInfo;
};

} // namespace tzw

#endif // TZW_CHUNKMGR_H
