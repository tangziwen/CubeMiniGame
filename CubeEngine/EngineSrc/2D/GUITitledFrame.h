#ifndef TZW_GUITITLEDFRAME_H
#define TZW_GUITITLEDFRAME_H
#include "GUIFrame.h"
#include "Button.h"
#include "LabelNew.h"
#include <string>
#include "../Event/Event.h"
namespace tzw {

class GUIWindow : public GUIFrame, public EventListener
{
public:
    static GUIWindow * create(std::string titleText, vec4 color,vec2 size);
    static GUIWindow * create(std::string titleText,vec2 size);
    GUIWindow();
    std::string title() const;
    void setTitle(const std::string &title);
    virtual void setContentSize(const vec2 &getContentSize);
    virtual vec2 getOuterContentSize();
    virtual bool onMouseRelease(int button,vec2 pos);
    virtual bool onMousePress(int button,vec2 pos);
    virtual bool onMouseMove(vec2 pos);
    bool getIsFocus() const;
    void setIsFocus(bool isFocus);
    bool getIsDragable() const;
    void setIsDragable(bool isDragable);
    bool getIsShowCloseBtn() const;
    void setIsShowCloseBtn(bool isShowCloseBtn);
	Button * getCloseBtn();

protected:
    void initLabel();
    void initCloseBtn();
    void initBorders();
    void adjustBorders();
    void adjustCloseBtn();
    bool m_isFocus;
    bool m_isDragable;
private:
    bool m_isShowCloseBtn;
    GUIFrame * m_labelFrame;
    GUIFrame * m_borders[4];
    Button * m_closeBtn;
    LabelNew * m_label;
    std::string m_title;
    vec2 m_preMousePos;
    bool m_isDraging;
};

} // namespace tzw

#endif // TZW_GUITITLEDFRAME_H
