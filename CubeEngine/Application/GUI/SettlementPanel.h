#ifndef TZWS_SETTLEMENTPANEL_H
#define TZWS_SETTLEMENTPANEL_H

#include "../EngineSrc/CubeEngine.h"
#include "../EngineSrc/2D/TableView.h"
namespace tzwS {

class SettlementPanel
{
public:
    SettlementPanel();
    void syncData();
    void toggle();
    void show();
    void hide();
private:
    void initPropertyTab();
    void initAdminTab();
    void initProductionTab();
    void initMilitaryTab();
    tzw::Node * m_propertyTab;
    tzw::Node * m_adminTab;
    tzw::Node * m_productionTab;
    tzw::Node * m_militaryTab;
    tzw::TableView * m_tabView;
    tzw::GUIWindow * m_frame;
    std::vector<tzw::LabelNew *> m_infoLabel;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENTPANEL_H
