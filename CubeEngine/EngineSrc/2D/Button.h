#ifndef TZW_BUTTON_H
#define TZW_BUTTON_H
#include "../2D/GUIFrame.h"
#include "../2D/LabelNew.h"
#include "../Event/Event.h"
#include "../Interface/Drawable2D.h"
#include <functional>
namespace tzw {

class Button: public Drawable2D,public EventListener
{
public:
    enum class Type
    {
        SimpleText,
        SimpleFrame,
    };
    Button();
    static Button * create(std::string str);
    static Button * create(vec4 color,vec2 Size);
    void setStr(std::string str);
    virtual bool onMouseRelease(int button,vec2 pos);
    virtual bool onMousePress(int button,vec2 pos);
    virtual bool onMouseMove(vec2 pos);
    std::function<void (Button *)> onBtnClicked() const;
    void setOnBtnClicked(const std::function<void (Button *)> &onBtnClicked);
    virtual vec2 getContentSize();
    LabelNew *getLabel() const;
    vec2 getFrameSize();
    void setFrameSize(vec2 size);
    void manualTrigger();

    GUIFrame *getFrameBG() const;

private:
    void init();
    void adjustBorders();
    void initBorders();
    GUIFrame * m_borders[4];
    Type m_type;
    std::function <void(Button *)> m_onBtnClicked;
    bool m_isTouched;
    GUIFrame * m_frameBG;
    LabelNew * m_label;
};

} // namespace tzw

#endif // TZW_BUTTON_H
