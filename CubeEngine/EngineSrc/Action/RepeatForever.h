#ifndef TZW_REPEATFOREVER_H
#define TZW_REPEATFOREVER_H

#include "Action.h"
#include "ActionInterval.h"
namespace tzw {

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
