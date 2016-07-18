#include "SettlementPanel.h"
#include "../GameSystem.h"
#include "../MapSystem.h"
using namespace tzw;
namespace tzwS {
#define LABEL_NAME 0
#define LABEL_PUBLIC_ORDER 1
#define LABEL_POPULATION 2
#define LABEL_DEVELOPMENT 3
#define LABEL_MILPERFORMANCE 4
#define LABEL_DEFENCE 5
SettlementPanel::SettlementPanel()
{
    auto size = Engine::shared()->winSize();
    m_frame = GUIWindow::create("settlement",vec2(450,500));
    m_frame->setPos2D(size.x/2 - 225,size.y/2 - 250);

    m_tabView = new TableView(vec2(450,500));
    m_propertyTab = m_tabView->addTab("属性");
    m_adminTab = m_tabView->addTab("行政");
    m_productionTab = m_tabView->addTab("经济");
    m_militaryTab = m_tabView->addTab("军事");
    m_frame->addChild(m_tabView);
    GUIWindowMgr::shared()->add(m_frame);

    initPropertyTab();
    initAdminTab();
    initProductionTab();
    initMilitaryTab();
}

void SettlementPanel::syncData()
{
    auto cell = MapSystem::shared()->getCurrentSelectedCell();
    if (cell != nullptr)
    {
        auto settlement = cell->settlement();
        static char tmp[50];
        sprintf(tmp,"name:%s",settlement->name().c_str());
        m_infoLabel[LABEL_NAME]->setString(tmp);

        sprintf(tmp,"Public Order:%g",T_GET(settlement,"PublicOrder"));
        m_infoLabel[LABEL_PUBLIC_ORDER]->setString(tmp);

        sprintf(tmp,"Population:%g",T_GET(settlement,"Population"));
        m_infoLabel[LABEL_POPULATION]->setString(tmp);

        sprintf(tmp,"Development:%g",T_GET(settlement,"Development"));
        m_infoLabel[LABEL_DEVELOPMENT]->setString(tmp);

        sprintf(tmp,"Millitary Performance:%g",T_GET(settlement,"MillitaryPerformance"));
        m_infoLabel[LABEL_MILPERFORMANCE]->setString(tmp);

        sprintf(tmp,"Defence:%g",T_GET(settlement,"Defence"));
        m_infoLabel[LABEL_DEFENCE]->setString(tmp);
    }
}

void SettlementPanel::toggle()
{
    if(m_frame->getIsDrawable())
    {
        hide();
    }else
    {
        show();
    }
}

void SettlementPanel::show()
{
    m_frame->setIsDrawable(true);
}

void SettlementPanel::hide()
{
    m_frame->setIsDrawable(false);
}

void SettlementPanel::initPropertyTab()
{
    for(int i =0 ;i <= LABEL_DEFENCE; i++)
    {
        auto label = LabelNew::create("null str");
        label->setPos2D(30,500 - (i * 30 + 25));
        m_propertyTab->addChild(label);
        m_infoLabel.push_back(label);
    }
}

void SettlementPanel::initAdminTab()
{
    Button * startBtn = Button::create("行政命令A");
    startBtn->setPos2D(100,340);
    m_adminTab->addChild(startBtn);

    startBtn = Button::create("行政命令B");
    startBtn->setPos2D(100,300);
    m_adminTab->addChild(startBtn);


    startBtn = Button::create("行政命令C");
    startBtn->setPos2D(100,260);
    m_adminTab->addChild(startBtn);

    startBtn = Button::create("行政命令D");
    startBtn->setPos2D(100,220);
    m_adminTab->addChild(startBtn);
}

void SettlementPanel::initProductionTab()
{

}

void SettlementPanel::initMilitaryTab()
{

}

} // namespace tzwS
