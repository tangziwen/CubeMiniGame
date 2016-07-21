#include "TopBar.h"
using namespace tzw;
#include "../GameSystem.h"
namespace tzwS {
#define LABEL_DATE 0
#define LABEL_MONEY 1
#define LABEL_FOOD 2
#define LABEL_ADMIN 3
#define LABEL_MIL 4
#define LABEL_TECH 5
TopBar::TopBar()
{
    auto winSize = Engine::shared()->winSize();
    m_frame = GUIFrame::create(vec4(0.1,0.1,0.1,1.0),vec2(winSize.x,50));
    m_frame->setPos2D(0,winSize.y - 50);
    SceneMgr::shared()->currentScene()->addNode(m_frame);

    for(int i =0 ;i <= LABEL_TECH; i++)
    {
        auto label = LabelNew::create("null str");
        label->setPos2D(100 + 150 * i,25);
        m_frame->addChild(label);
        m_infoLabel.push_back(label);
    }

    m_actBtn = Button::create("start");
    m_actBtn->setPos2D(10,15);
    m_actBtn->setOnBtnClicked([](Button * button)
    {
        (void)button;
        GameSystem::shared()->infoPanel()->toggle();
    });
    m_frame->addChild(m_actBtn);
    syncData();
}

void TopBar::syncData()
{
    static char tmp[50];
    m_infoLabel[LABEL_DATE]->setString(calDate());
    sprintf(tmp,"money:%d",GameSystem::shared()->player()->currency.money);
    m_infoLabel[LABEL_MONEY]->setString(tmp);
    sprintf(tmp,"food:%d",GameSystem::shared()->player()->currency.food);
    m_infoLabel[LABEL_FOOD]->setString(tmp);
    sprintf(tmp,"admin:%d",GameSystem::shared()->player()->currency.admin);
    m_infoLabel[LABEL_ADMIN]->setString(tmp);
    sprintf(tmp,"mil:%d",GameSystem::shared()->player()->currency.mil);
    m_infoLabel[LABEL_MIL]->setString(tmp);
    sprintf(tmp,"tech:%d",GameSystem::shared()->player()->currency.tech);
    m_infoLabel[LABEL_TECH]->setString(tmp);
}

std::string monthStr[] = {"元","二","三","四","五","六","七","八","九","十","十一","十二"};

std::string TopBar::calDate()
{
    static char tmp[50];
    auto totalDate = GameSystem::shared()->date();
    auto year = totalDate / 365;
    totalDate = totalDate - year * 365;
    auto month = totalDate / 30;
    totalDate = totalDate - month * 30;
    auto date = totalDate;
    sprintf(tmp,"%d年%s月%d日",year + 1,monthStr[month].c_str(),date + 1);
    return tmp;
}

} // namespace tzwS
