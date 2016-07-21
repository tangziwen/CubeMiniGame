#include "SettlementPanel.h"
#include "../GameSystem.h"
#include "../MapSystem.h"
#include "../EngineSrc/Font/FontMgr.h"
#include "CurrencyLabel.h"
#include "../EngineSrc/2D/GUIAttributeLabel.h"
using namespace tzw;
namespace tzwS {
#define LABEL_NAME 0
#define LABEL_PUBLIC_ORDER 1
#define LABEL_POPULATION 2
#define LABEL_DEVELOPMENT 3
#define LABEL_MILPERFORMANCE 4
#define LABEL_DEFENCE 5

vec4 hightLightedColor = vec4(0.3,0.1,0.1,1.0);
vec4 normalColor = vec4(0.1,0.3,0.1,1.0);
SettlementPanel::SettlementPanel()
{
    auto size = Engine::shared()->winSize();
    m_frame = GUIWindow::create("settlement",vec2(750,800));

    m_tabView = new TableView(vec2(750,780));
    //    m_propertyTab = m_tabView->addTab("属性");
    //    m_adminTab = m_tabView->addTab("行政");
    m_productionTab = m_tabView->addTab("生产");
    m_militaryTab = m_tabView->addTab("军事");
    m_frame->addChild(m_tabView);
    m_currentSelectedCell = nullptr;
    GUIWindowMgr::shared()->add(m_frame);

    //initPropertyTab();
    //initAdminTab();
    initProductionTab();
    initMilitaryTab();
}

void SettlementPanel::syncData()
{
    syncProductionTab();
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

void SettlementPanel::syncProductionTab()
{
    auto selectedCell = MapSystem::shared()->getCurrentSelectedCell();
    if(!selectedCell)
    {
        return;
    }

    auto currSettlement = MapSystem::shared()->getCurrentSelectedCell()->settlement();
    if(!currSettlement)
    {
        return;
    }
    static char tmp[50];
    sprintf(tmp,"name:%s",currSettlement->name().c_str());
    m_infoLabel[LABEL_NAME]->setString(tmp);

    sprintf(tmp,"Public Order:%g",T_GET(currSettlement,"PublicOrder"));
    m_infoLabel[LABEL_PUBLIC_ORDER]->setString(tmp);

    sprintf(tmp,"Population:%g",T_GET(currSettlement,"Population"));
    m_infoLabel[LABEL_POPULATION]->setString(tmp);

    sprintf(tmp,"Development:%g",T_GET(currSettlement,"Development"));
    m_infoLabel[LABEL_DEVELOPMENT]->setString(tmp);

    sprintf(tmp,"Millitary Performance:%g",T_GET(currSettlement,"MillitaryPerformance"));
    m_infoLabel[LABEL_MILPERFORMANCE]->setString(tmp);

    sprintf(tmp,"Defence:%g",T_GET(currSettlement,"Defence"));
    m_infoLabel[LABEL_DEFENCE]->setString(tmp);

    auto t = SETTLEMENT_CELL_SIZE;
    for(int k =0; k<t; k++)
    {
        auto cell = currSettlement->getCellIndex(k);
        auto frame = m_productionTab->getChildByTag(k);
        auto nameLabel = dynamic_cast<LabelNew*>(frame->getChildByName("nameLabel"));
        nameLabel->setString(cell->name());
        auto currencyLable = static_cast<CurrencyLabel*>(frame->getChildByName("currencyLabel"));
        if(cell->building() != nullptr)
        {
            currencyLable->syncData(cell->building()->boost() + cell->boost());
        }else
        {
            currencyLable->syncData(cell->boost());
        }
    }
}

void SettlementPanel::initPropertyTab()
{

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


    auto t = SETTLEMENT_CELL_SIZE;
    int marginX = 35;
    int marginY = 35;
    int advanceX = marginX;
    int advanceY = marginY;
    int gridSize = 4;
    for(int i =0; i<t; i++)
    {
        if(i % gridSize == 0)
        {
            if(i!= 0)
            {
                advanceX = marginX;
                advanceY += 140;
            }
        }
        else
        {
            advanceX += 140;
        }
        auto frame = Button::create(normalColor,vec2(135,135));
        frame->setOnBtnClicked(std::bind(&SettlementPanel::onCellClicked,this,std::placeholders::_1));
        frame->setPos2D(advanceX,advanceY);
        frame->setTag(i);
        auto nameLabel = LabelNew::create("empty",FontMgr::shared()->getSmallFont());
        nameLabel->setName("nameLabel");
        nameLabel->setPos2D(5,80);
        frame->addChild(nameLabel);

        auto currencyLabel = new CurrencyLabel(Currency(),true,false);
        currencyLabel->setName("currencyLabel");
        frame->addChild(currencyLabel);
        m_productionTab->addChild(frame);

    }
    auto buildBtn = Button::create("建造");
    buildBtn->setPos2D(600,350);
    m_productionTab->addChild(buildBtn);

    auto removeBtn = Button::create("拆毁");
    removeBtn->setPos2D(600,310);
    m_productionTab->addChild(removeBtn);


    for(int i =0 ;i <= LABEL_DEFENCE; i++)
    {
        auto label = LabelNew::create("null str");
        label->setPos2D(30,700 - (i * 30 + 25));
        m_productionTab->addChild(label);
        m_infoLabel.push_back(label);
    }
    auto test = GUIAttributeLabel::createWithFixedSize("name",100,vec2(130,30));
    m_productionTab->addChild(test);
}

void SettlementPanel::initMilitaryTab()
{

}

void SettlementPanel::onCellClicked(Button *btn)
{
    btn->getFrameBG()->setUniformColor(hightLightedColor);
    if(m_currentSelectedCell)
    {
        m_currentSelectedCell->getFrameBG()->setUniformColor(normalColor);
    }
    m_currentSelectedCell = btn;
}

} // namespace tzwS
