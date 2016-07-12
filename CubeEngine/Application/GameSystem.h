#ifndef TZWS_GAMESYSTEM_H
#define TZWS_GAMESYSTEM_H
#include "../EngineSrc/Engine/EngineDef.h"
#include "Hero.h"
namespace tzwS {

class GameSystem
{

public:
    GameSystem();
    void startNewGame();
    Hero *player() const;
    void setPlayer(Hero *player);
private:
    Hero * m_player;
    TZW_SINGLETON_DECL(GameSystem)
};

} // namespace tzwS

#endif // TZWS_GAMESYSTEM_H
