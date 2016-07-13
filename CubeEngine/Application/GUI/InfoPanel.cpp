#include "InfoPanel.h"
using namespace tzw;
#include "../GameSystem.h"
namespace tzwS {
#define LABEL_NAME 0
#define LABEL_MONEY 1
#define LABEL_LAND 2
#define LABEL_VASSAL 3
InfoPanel::InfoPanel()
{

    auto size = Engine::shared()->winSize();
    m_frame = GUIWindow::create("info Panel",vec2(450,500));
    m_frame->setPos2D(size.x/2 - 225,size.y/2 - 250);
    GUIWindowMgr::shared()->add(m_frame);
    for(int i =0 ;i <= LABEL_VASSAL; i++)
    {
        auto label = LabelNew::create("AAAAA");
        label->setPos2D(30,500 - (i * 30 + 25));
        m_frame->addChild(label);
        m_infoLabel.push_back(label);
    }
    //    m_frame->addChild(startBtn);
}

void InfoPanel::syncData()
{
    static char tmp[50];

    m_infoLabel[LABEL_NAME]->setString(GameSystem::shared()->player()->name());
    sprintf(tmp,"%d",GameSystem::shared()->player()->money());
    m_infoLabel[LABEL_MONEY]->setString(tmp);
}

} // namespace tzwS
