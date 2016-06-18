#ifndef TZW_ACTIONMGR_H
#define TZW_ACTIONMGR_H

#include "Action.h"
#include <list>


namespace tzw {
class Node;
class ActionMgr
{
public:
    ActionMgr();
    void runAction(Action * action);
    Action * getActionByIndex(unsigned int index);
    unsigned int getActionAmount();
    void stopAction(Action * action);
    void stopAllAction();
    void updateAction(Node * node, float dt);
protected:
    std::list<Action *> m_actionList;
};

} // namespace tzw

#endif // TZW_ACTIONMGR_H
