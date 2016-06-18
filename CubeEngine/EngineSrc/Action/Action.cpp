#include "Action.h"
#include "../Base/Node.h"
namespace tzw {

Action::Action()
{

}

bool Action::isDone()
{
    return true;
}
///
/// \brief 内部更新函数，请不要瞎鸡巴乱搞，如果要注册帧更新请使用step函数
/// \param node
/// \param dt
///
void Action::update(Node *node, float dt)
{
    step(node,dt);
}

///
/// \brief 开放给用户使用的帧更新函数
/// \param node
/// \param dt
///
void Action::step(Node *node, float dt)
{

}

} // namespace tzw
