#ifndef TZW_GUILISTVIEW_H
#define TZW_GUILISTVIEW_H

#include "../Interface/Drawable2D.h"
#include "GUIListViewItem.h"
#include <functional>
#include <vector>
#include "../2D/GUIFrame.h"
#include "Button.h"
#include "../Event/Event.h"
#include "../2D/LabelNew.h"
namespace tzw {

class GUIListView : public Drawable2D,public EventListener
{
public:
    GUIListView();
    static GUIListView * create(vec2 theSize);
    void addItem(GUIListViewItem * item);
    GUIListViewItem * add(std::string name,std::function<void(GUIListViewItem*)> cb = nullptr);
    virtual bool onMouseRelease(int button,vec2 pos);
    virtual bool onMousePress(int button,vec2 pos);
    virtual bool onMouseMove(vec2 pos);
    void nextPage();
    void prevPage();
    void updateForPage();
    void finalize();
private:
    void clearHightLight();
    void setItemHightLight(int theCurrentIndex);
    void resetItemHightLight();
    void initUI(vec2 theSize);
    std::vector<GUIListViewItem *>m_itemList;
    int m_currSelectedIndex;
    GUIFrame * m_frame;
    Button * m_prevBtn;
    Button * m_nextBtn;
    int m_btnSize;
    bool m_isClicking;
    vec2 m_margin;
    vec2 m_size;
    int m_pageSize;
    int m_currentPage;
    int m_itemSpace;
    std::vector<LabelNew * > m_itemLabel;
};

} // namespace tzw

#endif // TZW_GUILISTVIEW_H
