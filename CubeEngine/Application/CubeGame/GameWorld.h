#ifndef TZW_CHUNKMGR_H
#define TZW_CHUNKMGR_H

#include "EngineSrc/CubeEngine.h"
#include "./Chunk.h"
#include "Player.h"
#include "MainMenu.h"
#include <set>
namespace tzw {
#define CUBE_MAP_SIZE 128
#define WORLD_HEIGHT 1
class GameWorld : public EventListener
{
public:
    enum class GameState
    {
        MainMenu,
        OnPlay,
    };

    static GameWorld * shared();
    void createWorld(Scene* scene, int blockWitdh, int blockHeight, float ratio);
    vec3 worldToGrid(vec3 world);
    vec3 gridToChunk(vec3 grid);
    Chunk * getChunkByChunk(int x,int y,int z);
    Block * getBlockByGrid(int x,int y,int z);
    Player *getPlayer() const;
    void setPlayer(Player *player);
    Chunk * getOrCreateChunk(int x,int y, int z);
    virtual void onFrameUpdate(float delta);
    Chunk * createChunk(int x,int y,int z);
    void loadBlockSheet();
    TextureAtlas *getBlockSheet() const;
    void setBlockSheet(TextureAtlas *blockSheet);
    bool isTheSurfaceBlock();
    void startGame();
    virtual bool onKeyPress(int keyCode);
    MainMenu *getMainMenu() const;
    void toggleMainMenu();
    void unloadGame();
    GameState getCurrentState() const;
    void setCurrentState(const GameState &currentState);

    Node *getMainRoot() const;
    void setMainRoot(Node *mainRoot);

private:
    Node * m_mainRoot;
    GameState m_currentState;
    void loadChunksAroundPlayer();
    int m_width,m_height;
    Chunk * m_chunkArray[CUBE_MAP_SIZE][CUBE_MAP_SIZE][WORLD_HEIGHT];
    TextureAtlas *m_blockSheet;
    Scene  * m_scene;
    Player * m_player;
    Sprite * crossHair;
    std::vector<Chunk*> m_chunkList;
    static GameWorld *m_instance;
    GameWorld();
    std::set<Chunk*> m_activedChunkList;
    MainMenu * m_mainMenu;
};

} // namespace tzw

#endif // TZW_CHUNKMGR_H
