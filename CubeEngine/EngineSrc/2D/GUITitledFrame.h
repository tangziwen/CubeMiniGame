#ifndef TZW_GUITITLEDFRAME_H
#define TZW_GUITITLEDFRAME_H
#include "GUIFrame.h"
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
protected:
    void initLabel();
    void initBorders();
    void adjustBorders();
    bool m_isFocus;
    bool m_isDragable;
private:
    GUIFrame * m_labelFrame;
    GUIFrame * m_borders[4];
    LabelNew * m_label;
    std::string m_title;
    vec2 m_preMousePos;
    bool m_isDraging;
};

} // namespace tzw

#endif // TZW_GUITITLEDFRAME_H
