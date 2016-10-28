#ifndef TZW_EVENT_H
#define TZW_EVENT_H
#include "../Math/vec2.h"
#include <string>
#include <GLFW/glfw3.h>

namespace tzw {
class Node;
class EventMgr;

/**
 * @brief The EventListener class 用于监听输入事件
 * EventListner 用于监听各种各样的系统输入如鼠标点击，拖动，键盘按键等
 * 为了方便起见，EventListener有两种形式，：
 * (1) 第一种是使用固定的优先级，当事件发生时，根据优先级的先后，来依次处理事件。
 * (2) 跟随一个Node的优先级，这种情况下，EventMgr首先会根据Node的层次结构进行一次遍历得到一个符合局部优先级(Local Piority)的list，
 * 然后再会根据各Node不同的全局优先级(Global Piority)对该list进行一次稳定排序，得出最终的顺序。
 * 请注意，和Node的优先级可能不同的是，对于2D渲染而言，低Piority先处理(绘制)，高piority后处理(绘制)的图元会叠在先处理之上，而对于EventListener来说，高Piority的先处理
 * 这二者处理顺序是完全反过来的，但是又有特殊的作用：举例说明：两个叠放在一起的按钮，高优先级(上方的)后绘制，但是在鼠标点击时，优先响应
 */
class EventListener
{
public:
    virtual bool onKeyPress(int keyCode);
    virtual ~EventListener();
    virtual bool onKeyRelease(int keyCode);
    virtual bool onCharInput(unsigned int theChar);
    virtual bool onMouseRelease(int button,vec2 pos);
    virtual bool onMousePress(int button,vec2 pos);
    virtual bool onMouseMove(vec2 pos);
    virtual void onFrameUpdate(float delta);
    EventListener();
    unsigned int getFixedPiority() const;
    void setFixedPiority(unsigned int getFixedPiority);
    bool isSwallow() const;
    void setIsSwallow(bool isSwallow);
    EventMgr *parent() const;
    void setParent(EventMgr *parent);
    Node *attachNode() const;
    void setAttachNode(Node *attachNode);
protected:
    Node * m_attachNode;
    EventMgr * m_parent;
    bool m_isSwallow;
    unsigned int m_fixedPiority;
};

} // namespace tzw

#endif // TZW_EVENT_H
