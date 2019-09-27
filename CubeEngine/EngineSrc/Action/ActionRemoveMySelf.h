#pragma once
#include "Action.h"
namespace tzw {
class Node;
///
/// \brief 动作类，用于实现对于Node（以及其子类）持续一段时间或永久的影响，比如移动，旋转，缩放等
///
class ActionRemoveMySelf: public Action
{
public:
    ActionRemoveMySelf();
    void update(Node * node, float dt) override;
    virtual bool isDone();
private:
	bool m_isOk;
};

} // namespace tzw

