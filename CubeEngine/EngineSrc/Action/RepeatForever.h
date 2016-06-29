#ifndef TZW_REPEATFOREVER_H
#define TZW_REPEATFOREVER_H

#include "Action.h"
#include "ActionInterval.h"
namespace tzw {

///
/// \brief The RepeatForever class 用于重复执行某一动作的包装动作
/// 其内部包含了一个需要被执行的动作，并在其执行完毕后，是其再次重新执行
///
class RepeatForever : public Action
{
public:
    RepeatForever(ActionInterval * act);

    virtual void init(Node * theNode);
    virtual void update(Node * node, float dt);
    virtual bool isDone();
    virtual ~RepeatForever();
private:
    ActionInterval * m_internalAct;
};

} // namespace tzw

#endif // TZW_REPEATFOREVER_H
