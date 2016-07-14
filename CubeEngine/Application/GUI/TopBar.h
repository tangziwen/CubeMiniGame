#ifndef TZWS_TOPBAR_H
#define TZWS_TOPBAR_H
#include "../EngineSrc/CubeEngine.h"

namespace tzwS {

class TopBar
{
public:
    TopBar();
    void syncData();
private:
    std::vector<tzw::LabelNew *> m_infoLabel;
    tzw::GUIFrame * m_frame;
    tzw::Button * m_actBtn;
};

} // namespace tzwS

#endif // TZWS_TOPBAR_H
