#include "SimMainMenu.h"

#include "EngineSrc/CubeEngine.h"
#include "SimGame/SimGameSystem.h"
namespace tzw {

static void exitNow(Button * btn)
{
    exit(0);
}

static void onOption(Button * btn)
{

}

SimMainMenu::SimMainMenu()
{
    m_mainFrame = GUIWindow::create("SimGame",vec2(300,400));
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

void SimMainMenu::show()
{
    setIsVisible(true);
}

void SimMainMenu::hide()
{
    setIsVisible(false);
}

void SimMainMenu::toggle()
{
    if (getIsVisible())
    {
        hide();
    }else
    {
        show();
    }
}

void SimMainMenu::startGame(Button * btn)
{
	SimGameSystem::shared()->startGame();
    hide();
}

} // namespace tzw
