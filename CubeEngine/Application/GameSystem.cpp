#include "GameSystem.h"
#include "MapSystem.h"
#include "Hero.h"
#include "Player.h"
namespace tzwS {
TZW_SINGLETON_IMPL(GameSystem)

GameSystem::GameSystem()
{
    m_player = new Player();
    m_state = State::MainMenu;
}

void GameSystem::startNewGame()
{
    m_infoPanel = new InfoPanel();
    m_infoPanel->hide();
    m_topBar = new TopBar();
    m_settlementPanel = new SettlementPanel();
    m_settlementPanel->hide();
    MapSystem::shared()->init();
    MapSystem::shared()->addSettlement(5,0,new Settlement("guangzhou"));
    MapSystem::shared()->addSettlement(0,4,new Settlement("guilin"));
    MapSystem::shared()->addSettlement(7,8,new Settlement("shanghai"));
    MapSystem::shared()->addSettlement(5,15,new Settlement("beijing"));

    auto xlj = new Hero("rzx");
    addHero(xlj);
    xlj->own("shanghai");
    auto rzx = new Hero("xlj");
    rzx->own("beijing");
    addHero(rzx);
    m_player->own("guilin");
    m_player->own("guangzhou");
    MapSystem::shared()->initGraphics();
    m_state = State::InGame;
    m_date = 0;
}

Hero *GameSystem::player() const
{
    return m_player;
}

void GameSystem::setPlayer(Hero *player)
{
    m_player = player;
}

void GameSystem::update()
{
    if(m_state != State::InGame) return;
    m_date +=1;
    m_infoPanel->syncData();
    m_topBar->syncData();
    m_settlementPanel->syncData();
    MapSystem::shared()->update();
    m_player->update();
    for(auto hero:m_heroList)
    {
        hero->update();
    }
}

void GameSystem::addHero(Hero *theHero)
{
    m_heroList.push_back(theHero);
}

unsigned int GameSystem::date() const
{
    return m_date;
}

InfoPanel *GameSystem::infoPanel() const
{
    return m_infoPanel;
}

TopBar *GameSystem::topBar() const
{
    return m_topBar;
}

SettlementPanel *GameSystem::settlementPanel() const
{
    return m_settlementPanel;
}

} // namespace tzwS
