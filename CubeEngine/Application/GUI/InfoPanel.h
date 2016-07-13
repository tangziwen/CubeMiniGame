#ifndef TZWS_INFOPANEL_H
#define TZWS_INFOPANEL_H


#include "../EngineSrc/CubeEngine.h"
namespace tzwS {
class InfoPanel
{
public:
    InfoPanel();
    void syncData();
private:

    tzw::GUIWindow * m_frame;
    std::vector<tzw::LabelNew *> m_infoLabel;
};

} // namespace tzwS

#endif // TZWS_INFOPANEL_H
