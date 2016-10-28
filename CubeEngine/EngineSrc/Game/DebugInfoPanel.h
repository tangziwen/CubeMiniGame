#ifndef TZW_DEBUGINFOPANEL_H
#define TZW_DEBUGINFOPANEL_H
#include "../2D/GUITitledFrame.h"
namespace tzw {
class LabelNew;
class DebugInfoPanel : public Node
{
public:
    DebugInfoPanel();
    virtual void logicUpdate(float dt);
    void setInfo();
private:
    GUIWindow * m_frame;
    std::vector<LabelNew*> m_labelList;
    float m_curTime;
    bool m_isInit;
};

} // namespace tzw

#endif // TZW_DEBUGINFOPANEL_H
