#include "MainMenu.h"

#include "EngineSrc/CubeEngine.h"
#include "CubeGame/GameWorld.h"
namespace tzw {

static void exitNow(Button * btn)
{
    exit(0);
}

static void onOption(Button * btn)
{

}

MainMenu::MainMenu()
{
    m_mainFrame = GUIWindow::create("menu",vec2(300,400));
    auto size = m_mainFrame->getContentSize();
    m_mainFrame->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
    addChild(m_mainFrame);

    Button * startBtn = Button::create("start");
    startBtn->setPos2D(100,340);
    startBtn->setOnBtnClicked([=](Button * btn)->void{this->startGame(btn);});
    m_mainFrame->addChild(startBtn);

    Button * optionBtn = Button::create("option");
    optionBtn->setPos2D(100,290);
    optionBtn->setOnBtnClicked(std::bind(&onOption,std::placeholders::_1));
    m_mainFrame->addChild(optionBtn);

    Button * quitBtn = Button::create("exit");
    quitBtn->setPos2D(100,240);
    quitBtn->setOnBtnClicked(&exitNow);
    m_mainFrame->addChild(quitBtn);
}

void MainMenu::show()
{
    setIsDrawable(true);
}

void MainMenu::hide()
{
    setIsDrawable(false);
}

void MainMenu::toggle()
{
    if (getIsDrawable())
    {
        hide();
    }else
    {
        show();
    }
}

void MainMenu::startGame(Button * btn)
{
    GameWorld::shared()->startGame();
    hide();
	Engine::shared()->setUnlimitedCursor(true);
}

} // namespace tzw
