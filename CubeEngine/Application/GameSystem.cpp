#include "GameSystem.h"
#include "MapSystem.h"
namespace tzwS {
TZW_SINGLETON_IMPL(GameSystem)

GameSystem::GameSystem()
{

}

void GameSystem::startNewGame()
{
    MapSystem::shared()->init();
    MapSystem::shared()->addSettlement(5,0,new Settlement("广州"));
    MapSystem::shared()->addSettlement(0,4,new Settlement("桂林"));
    MapSystem::shared()->addSettlement(7,8,new Settlement("上海"));
    MapSystem::shared()->addSettlement(5,15,new Settlement("北京"));
    MapSystem::shared()->initGraphics();
}

Hero *GameSystem::player() const
{
    return m_player;
}

void GameSystem::setPlayer(Hero *player)
{
    m_player = player;
}
} // namespace tzwS
