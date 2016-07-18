#ifndef TZW_TABLEVIEW_H
#define TZW_TABLEVIEW_H

#include <string>
#include <vector>
#include "GUIFrame.h"
#include "Button.h"
namespace tzw {

class TableView: public Drawable
{
public:
    TableView(vec2 size);
    Node * addTab(std::string tabName);
    Node * getTabByName(std::string tabName);
    void initTabButtons();
    void onTabBtnClicked(Button * button);
    void focus(unsigned int index);
protected:
    std::vector<Node *>m_tableList;
    std::vector<Button *>m_tabBtnList;
    tzw::Node * m_currentFocusNode;
    GUIFrame * m_frame;
    GUIFrame * m_buttonFrame;
};

} // namespace tzw

#endif // TZW_TABLEVIEW_H
