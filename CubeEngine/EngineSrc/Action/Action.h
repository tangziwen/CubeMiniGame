#ifndef TZW_ACTION_H
#define TZW_ACTION_H


namespace tzw {
class Node;
///
/// \brief 动作类，用于实现对于Node（以及其子类）持续一段时间或永久的影响，比如移动，旋转，缩放等
///
class Action
{
public:
    Action();
    virtual bool isDone();
    virtual void update(Node * node, float dt);
    virtual void step(Node * node,float dt);
};

} // namespace tzw

#endif // TZW_ACTION_H
