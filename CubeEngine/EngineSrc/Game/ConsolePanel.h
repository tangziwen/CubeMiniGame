#ifndef TZW_CONSOLEPANEL_H
#define TZW_CONSOLEPANEL_H

#include "../2D/LabelNew.h"
#include "../2D/Sprite.h"
#include "../Event/Event.h"
#include <deque>

namespace tzw {

///用于输出调试信息，以及输入脚本的控制台窗口
class ConsolePanel : EventListener
{
public:
    ConsolePanel(Node * renderNode);
    virtual bool onKeyPress(int keyCode);
    virtual bool onCharInput(unsigned int theChar);
    bool isVisible() const;
    void setIsVisible(bool isVisible);
    void toggleVissible();
    void print(std::string str);
    unsigned int verticalSpace() const;
    void setVerticalSpace(unsigned int verticalSpace);

    vec2 margin() const;
    void setMargin(vec2 margin);

    float heightRatio() const;
    void setHeightRatio(float heightRatio);
private:
    void updateTexts();
    void eraseChar();
	void parse(std::string theStr);
    int maxList() const;
    unsigned int m_verticalSpace;
    unsigned int m_inputFrameHeight;
    vec2 m_margin;
    std::deque<LabelNew *> m_labelList;
    bool m_isVisible;
    Node * m_node;
    LabelNew * m_label;
    Sprite * m_bgFrame;
    Sprite * m_inputFrame;
    float m_heightRatio;
};

} // namespace tzw

#endif // TZW_CONSOLEPANEL_H
