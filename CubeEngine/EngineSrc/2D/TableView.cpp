#include "TableView.h"
#include <algorithm>
namespace tzw {

TableView::TableView(vec2 size)
{
    m_currentFocusNode = nullptr;
    m_marginX = 15;
    m_frame = GUIFrame::create(size);
    addChild(m_frame);
    m_buttonFrame = GUIFrame::create(vec4(0.3,0.1,0.1,1.0),vec2(size.x,25));
    m_buttonFrame->setPos2D(0,size.y);
    addChild(m_buttonFrame);
}

Node *TableView::addTab(std::string tabName)
{
    auto node = Node::create();
    addChild(node);
    node->setName(tabName);
    m_tableList.push_back(node);
    initTabButtons();
    focus(m_tableList.size() - 1);
    return node;
}

Node *TableView::getTabByName(std::string tabName)
{
    for (auto node:m_tableList)
    {
        if(node->getName() == tabName)
        {
            return node;
        }
    }
    return nullptr;
}

void TableView::initTabButtons()
{
    Button * prev = nullptr;
    int index = 0;
    for(auto node : m_tableList)
    {
        auto button = Button::create(node->getName());
        m_buttonFrame->addChild(button);
        if(prev)
        {
            button->setPos2D(m_marginX + prev->getPos2D().x + prev->getContentSize().x + 10, 0);
        }else
        {
            button->setPos2D(m_marginX,0);
        }
        prev = button;
        button->setOnBtnClicked(std::bind(&TableView::onTabBtnClicked,this,std::placeholders::_1));
        button->setTag(index);
        index++;
    }
}

void TableView::onTabBtnClicked(Button *button)
{
    focus(button->getTag());
}

void TableView::focus(unsigned int index)
{
    if(m_currentFocusNode)
    {
        m_currentFocusNode->setIsDrawable(false);
    }
    auto node = m_tableList[index];
    node->setIsDrawable(true);
    m_currentFocusNode = node;
}

int TableView::getMarginX() const
{
    return m_marginX;
}

void TableView::setMarginX(int marginX)
{
    m_marginX = marginX;
}
} // namespace tzw
