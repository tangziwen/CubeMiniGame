#include "InfoPanel.h"
using namespace tzw;
#include "../GameSystem.h"
namespace tzwS {
#define LABEL_NAME 0
#define LABEL_LAND 1
#define LABEL_VASSAL 2
InfoPanel::InfoPanel()
{

    auto size = Engine::shared()->winSize();
    m_frame = GUIWindow::create("info Panel",vec2(450,500));
    m_frame->setPos2D(size.x/2 - 225,size.y/2 - 250);
    GUIWindowMgr::shared()->add(m_frame);
    for(int i =0 ;i <= LABEL_VASSAL; i++)
    {
        auto label = LabelNew::create("null str");
        label->setPos2D(30,500 - (i * 30 + 25));
        m_frame->addChild(label);
        m_infoLabel.push_back(label);
    }
    //m_frame->addChild(startBtn);
}

void InfoPanel::syncData()
{
    static char tmp[50];
    m_infoLabel[LABEL_NAME]->setString(GameSystem::shared()->player()->name());
}

void InfoPanel::toggle()
{
    if(m_frame->getIsDrawable())
    {
        hide();
    }else
    {
        show();
    }
}

void InfoPanel::show()
{
    m_frame->setIsDrawable(true);
}

void InfoPanel::hide()
{
    m_frame->setIsDrawable(false);
}

} // namespace tzwS
