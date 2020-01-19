#ifndef TZW_CHUNKMGR_H
#define TZW_CHUNKMGR_H

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
    void setBlockSheet(TextureAtlas *blockSheet);
    bool isTheSurfaceBlock();
    void startGame();
	void loadGame(std::string filePath);
	void saveGame(std::string filePath);
	bool onKeyPress(int keyCode) override;
    GameUISystem *getMainMenu() const;
    void unloadGame();
    int getCurrentState() const;
    void setCurrentState(const int &currentState);

    Node *getMainRoot() const;
    void setMainRoot(Node *mainRoot);
	void loadChunksAroundPlayer();
	tzw::vec3 getMapOffset() const;
	void setMapOffset(tzw::vec3 val);
	void init();
private:
    Node * m_mainRoot;
    int m_currentState;
    vec3 m_mapOffset;
    int m_width, m_depth, m_height;
    Chunk * m_chunkArray[128][128][16];
    Scene  * m_scene;
    CubePlayer * m_player;
    std::vector<Chunk*> m_chunkList;
    static GameWorld *m_instance;
    GameWorld();
    std::set<Chunk*> m_activedChunkList;
    GameUISystem * m_mainMenu;
	virtual ~GameWorld();
	void prepare();
};

} // namespace tzw

#endif // TZW_CHUNKMGR_H
