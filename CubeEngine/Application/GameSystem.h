#ifndef TZWS_GAMESYSTEM_H
#define TZWS_GAMESYSTEM_H
#include "../EngineSrc/Engine/EngineDef.h"
#include "Hero.h"
#include <vector>
#include "GUI/InfoPanel.h"
namespace tzwS {

class GameSystem
{

public:
    GameSystem();
    void startNewGame();
    Hero *player() const;
    void setPlayer(Hero *player);
    void update();
    void addHero(Hero * theHero);
private:
    std::vector<Hero *> m_heroList;
    Hero * m_player;
    InfoPanel * m_panel;
    TZW_SINGLETON_DECL(GameSystem)
};

} // namespace tzwS

#endif // TZWS_GAMESYSTEM_H
