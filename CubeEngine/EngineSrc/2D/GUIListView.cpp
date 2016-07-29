#include "GUIListView.h"
#include "GUIStyleMgr.h"
#include "../Event/EventMgr.h"
#include "../Font/FontMgr.h"
namespace tzw {

GUIListView::GUIListView()
{
    EventMgr::shared()->addNodePiorityListener(this,this);
    setIsSwallow(true);
    m_pageSize = 10;
    m_currentPage = 0;
    m_itemSpace = 15;
    m_margin = vec2(10,10);
    m_btnSize = 25;
    m_isClicking = false;
    m_currSelectedIndex = -1;
}

GUIListView *GUIListView::create(vec2 theSize)
{
    auto obj = new GUIListView();
    obj->initUI(theSize);
    return obj;
}

void GUIListView::addItem(GUIListViewItem *item)
{
    m_itemList.push_back(item);
}

GUIListViewItem *GUIListView::add(std::string name,std::function<void(GUIListViewItem*)> cb)
{
    auto obj = new GUIListViewItem();
    obj->m_str = name;
    obj->m_cb = cb;
    addItem(obj);
    return obj;
}

bool GUIListView::onMouseRelease(int button, vec2 pos)
{
    if(m_frame->isInTheRect(pos) && m_isClicking)
    {
        return true;
    }
    return false;
}

bool GUIListView::onMousePress(int button, vec2 pos)
{
    if(m_frame->isInTheRect(pos))
    {
        for(int i =0;i<m_pageSize;i++)
        {
            auto baseIndex = m_currentPage * m_pageSize;
            auto theIndex = baseIndex + i;
            if(theIndex >= m_itemList.size())
            {
                break;
            }
            auto frame = static_cast<GUIFrame *>(m_itemLabel[i]->getChildByTag(0));
            if(frame->isInTheRect(pos))
            {
                setItemHightLight(theIndex);
                break;
            }
        }
        m_isClicking = true;
        return true;
    }
    return false;
}

bool GUIListView::onMouseMove(vec2 pos)
{
    return false;
}

void GUIListView::nextPage()
{
    auto tmp = m_currentPage + 1;
    if(tmp * m_pageSize < m_itemList.size())
    {
        clearHightLight();//clear invalid hightlight
        m_currentPage ++;
        updateForPage();
    }
}

void GUIListView::prevPage()
{
    if(m_currentPage - 1 >= 0)
    {
        clearHightLight();//clear invalid hightlight
        m_currentPage --;
        updateForPage();
    }
}

void GUIListView::updateForPage()
{
    for(int i =0;i<m_pageSize;i++)
    {
        auto baseIndex = m_currentPage * m_pageSize;
        if(baseIndex + i >= m_itemList.size())
        {
            break;
        }
        m_itemLabel[i]->setString(m_itemList[baseIndex + i]->m_str);
    }
    setItemHightLight(m_currSelectedIndex);// set new hight light
}

void GUIListView::finalize()
{
    updateForPage();
    setItemHightLight(0);
}

void GUIListView::clearHightLight()
{
    if(m_currSelectedIndex == -1)
    {
        return;
    }
    auto baseIndex = m_currentPage * m_pageSize;
    auto relativeIndex = m_currSelectedIndex - baseIndex;
    if(relativeIndex >=0 && relativeIndex <m_pageSize)
    {
        auto frame = static_cast<GUIFrame *>(m_itemLabel[relativeIndex]->getChildByTag(0));
        frame->setIsDrawable(false);
    }
}


void GUIListView::setItemHightLight(int theCurrentIndex)
{
    if (theCurrentIndex == -1)
        return;
    auto baseIndex = m_currentPage * m_pageSize;
    auto preLelativeIndex = m_currSelectedIndex - baseIndex;
    auto relativeIndex = theCurrentIndex - baseIndex;
    if(relativeIndex >=0 && relativeIndex <m_pageSize)
    {

        if(m_currSelectedIndex != -1 && (preLelativeIndex>=0 && preLelativeIndex < m_pageSize))
        {
            auto prev = static_cast<GUIFrame *>(m_itemLabel[preLelativeIndex]->getChildByTag(0));
            prev->setIsDrawable(false);
        }
        m_currSelectedIndex = theCurrentIndex;
        auto frame = static_cast<GUIFrame *>(m_itemLabel[relativeIndex]->getChildByTag(0));
        frame->setIsDrawable(true);
    }
}

void GUIListView::resetItemHightLight()
{
    clearHightLight();
    m_currSelectedIndex = -1;
}

void GUIListView::initUI(vec2 theSize)
{
    m_size = theSize;
    m_frame =GUIFrame::create(GUIStyleMgr::shared()->defaultPalette()->backGroundColor, m_size);
    this->addChild(m_frame);
    m_prevBtn = Button::create("<");
    m_prevBtn->setFrameSize(vec2(m_btnSize,m_btnSize));
    m_prevBtn->setPos2D(2,2);
    m_nextBtn = Button::create(">");
    m_nextBtn->setFrameSize(vec2(m_btnSize,m_btnSize));
    m_nextBtn->setPos2D(theSize.x - m_btnSize - 2,2);
    m_frame->addChild(m_prevBtn);
    m_prevBtn->setOnBtnClicked([=](Button *){
        this->prevPage();
    });
    m_nextBtn->setOnBtnClicked([=](Button *){
        this->nextPage();
    });
    m_frame->addChild(m_nextBtn);
    auto fontSize = FontMgr::shared()->getDefaultFont()->getFontSize();
    for(int i = 0; i<m_pageSize;i++)
    {
        auto label = LabelNew::create("testItem");
        label->setAnchorPoint(vec2(0,1.0));
        auto theFrame =GUIFrame::create(vec4(0.3,0.3,0.1,0.5), vec2(theSize.x,fontSize + m_itemSpace));
        theFrame->setPos2D(-m_margin.x,-m_itemSpace/2);
        label->setPos2D(m_margin.x,theSize.y - m_margin.y - i * (fontSize + m_itemSpace));
        label->addChild(theFrame);
        theFrame->setTag(0);
        theFrame->setIsDrawable(false);
        m_frame->addChild(label);
        m_itemLabel.push_back(label);
    }
}

} // namespace tzw
