#ifndef TZW_BUTTON_H
#define TZW_BUTTON_H
#include "../Base/Node.h"
#include "../2D/GUIFrame.h"
#include "../2D/LabelNew.h"
#include "../Event/Event.h"
#include <functional>
namespace tzw {

class Button: public Node,public EventListener
{
public:
    Button();
    static Button * create(std::string str);
    void setStr(std::string str);
    virtual bool onMouseRelease(int button,vec2 pos);
    virtual bool onMousePress(int button,vec2 pos);
    virtual bool onMouseMove(vec2 pos);
    std::function<void (Button *)> onBtnClicked() const;
    void setOnBtnClicked(const std::function<void (Button *)> &onBtnClicked);
    vec2 getContentSize();
    LabelNew *getLabel() const;

    void manualTrigger();

private:
    std::function <void(Button *)> m_onBtnClicked;
    bool m_isTouched;
    GUIFrame * m_frameBG;
    LabelNew * m_label;
};

} // namespace tzw

#endif // TZW_BUTTON_H
