#include "DebugInfoPanel.h"
#include "../Engine/Engine.h"
#include "../2d/LabelNew.h"
#include "../Font/FontMgr.h"
#include "TUtility/TUtility.h"
#include "../Engine/EngineDef.h"

#include "../2D/GUIWindowMgr.h"
#define PANEL_WIDTH 220
#define PANEL_HEIGHT 180

#define LINE_SPACE 22

#define MARGIN_Y 25
#define MARGIN_X 5

#define LABEL_FPS 0
#define LABEL_DRAWCALLS 1
#define LABEL_LOGIC_UPDATE 2
#define LABEL_APPLYRENDER 3
#define LABEL_VERTICES_COUNT 4
#define LABEL_INDICES_COUNT 5
#define LABEL_VERSION 6

namespace tzw {
DebugInfoPanel::DebugInfoPanel()
    :m_curTime(0),m_isInit(false)
{
    m_frame = GUIWindow::create("Profile Window",vec2(PANEL_WIDTH,PANEL_HEIGHT));
    m_frame->setGlobalPiority(EngineDef::maxPiority);
    //m_frame->setIsDragable(false);
    GUIWindowMgr::shared()->add(m_frame);
    for (int i =0 ;i<= LABEL_VERSION;i++)
    {
        auto l = LabelNew::create(" ",FontMgr::shared()->getSmallFont());
        l->setPos2D(MARGIN_X,PANEL_HEIGHT - i * LINE_SPACE - MARGIN_Y);
        m_frame->addChild(l);
        m_labelList.push_back(l);
    }
    //版本做特殊处理，放在最底部
    m_labelList[LABEL_VERSION]->setPos2D(MARGIN_X,5);
    //加一点点透明度
    m_frame->setAlpha(0.7f);
    setInfo();
    m_isInit = true;
}

void DebugInfoPanel::update(float dt)
{
    m_curTime += dt;
    if(m_curTime > 0.5)
    {
        this->setInfo();
        m_curTime = 0.0f;
    }
}

void DebugInfoPanel::setInfo()
{
    static char tmp[100];
    float fps = 60.0f;
    int drawCallCount = 10;
    if(m_isInit)
    {
        fps = Engine::shared()->FPS();
        drawCallCount = Engine::shared()->getDrawCallCount();
    }

    sprintf_s(tmp,sizeof(tmp),"FPS:%f",fps);
    m_labelList[LABEL_FPS]->setString(tmp);

    sprintf_s(tmp,sizeof(tmp),"DrawCalls:%d",drawCallCount);
    m_labelList[LABEL_DRAWCALLS]->setString(tmp);

    sprintf_s(tmp,sizeof(tmp),"logicUpdate:%d ms",Engine::shared()->getLogicUpdateTime());
    m_labelList[LABEL_LOGIC_UPDATE]->setString(tmp);

    sprintf_s(tmp,sizeof(tmp),"applyRender:%d ms",Engine::shared()->getApplyRenderTime());
    m_labelList[LABEL_APPLYRENDER]->setString(tmp);

    sprintf_s(tmp,sizeof(tmp),"vertices:%d",Engine::shared()->getVerticesCount());
    m_labelList[LABEL_VERTICES_COUNT]->setString(tmp);

    sprintf_s(tmp,sizeof(tmp),"indices:%d",Engine::shared()->getIndicesCount());
    m_labelList[LABEL_INDICES_COUNT]->setString(tmp);

    m_labelList[LABEL_VERSION]->setString(EngineDef::versionStr);
}
} // namespace tzw
