#ifndef TZWS_SETTLEMENTPANEL_H
#define TZWS_SETTLEMENTPANEL_H

#include "../EngineSrc/CubeEngine.h"
#include "../EngineSrc/2D/TableView.h"
#include "../EngineSrc/2D/GUIAttributeLabel.h"
#include "../EngineSrc/2D/BoxContainer.h"
#include "GUi/PopUpMenu.h"
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
    void syncProductionTab();
    void initPropertyTab();
    void initAdminTab();
    void initProductionTab();
    void initMilitaryTab();
    void onCellClicked(tzw::Button * btn);
	void initPopUpMenu();
    tzw::Node * m_propertyTab;
    tzw::Node * m_adminTab;
    tzw::Node * m_productionTab;
    tzw::Node * m_militaryTab;
    tzw::TableView * m_tabView;
    tzw::GUIWindow * m_frame;
    std::vector<tzw::GUIAttributeLabel *> m_infoLabel;
    tzw::Button * m_currentSelectedCell;
    tzw::BoxContainer * m_infoContainer;
	tzw::BoxContainer * m_cellContainer;
	PopUpMenu * m_popMenu;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENTPANEL_H
