#ifndef TZWS_GAMESYSTEM_H
#define TZWS_GAMESYSTEM_H
#include "../EngineSrc/Engine/EngineDef.h"
#include "Hero.h"
#include <vector>
#include "GUI/InfoPanel.h"
#include "GUI/TopBar.h"
#include "GUI/SettlementPanel.h"
namespace tzwS {

class GameSystem
{

public:
    enum class State
    {
        MainMenu,
        InGame,
    };

    GameSystem();
    void startNewGame();
    Hero *player() const;
    void setPlayer(Hero *player);
    void update();
    void addHero(Hero * theHero);
    unsigned int date() const;

    InfoPanel *infoPanel() const;
    TopBar *topBar() const;

    SettlementPanel *settlementPanel() const;

private:
    unsigned int m_date;
    State m_state;
    std::vector<Hero *> m_heroList;
    Hero * m_player;
    SettlementPanel * m_settlementPanel;
    InfoPanel * m_infoPanel;
    TopBar * m_topBar;
    TZW_SINGLETON_DECL(GameSystem)
};

} // namespace tzwS

#endif // TZWS_GAMESYSTEM_H
