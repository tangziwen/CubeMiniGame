#include "ActionMgr.h"
#include <algorithm>
#include "../Base/Node.h"
namespace tzw {

ActionMgr::ActionMgr()
{

}

void ActionMgr::runAction(Action *action)
{
    m_actionList.push_back(action);
}

Action *ActionMgr::getActionByIndex(unsigned int index)
{
    int i =0;
    auto iter = m_actionList.begin();
    for(;i!=index && iter!= m_actionList.end();i++,iter++)
    {
    }
    if (iter != m_actionList.end())
    {
        return *iter;
    }else
    {
        return nullptr;
    }
}

unsigned int ActionMgr::getActionAmount()
{
    return m_actionList.size();
}

void ActionMgr::stopAction(Action *action)
{
    auto result = std::find(m_actionList.begin(),m_actionList.end(),action);
    if(result != m_actionList.end())
    {
        m_actionList.erase(result);
    }
    delete action;
}

void ActionMgr::stopAllAction()
{
    for(auto i = m_actionList.begin();i!= m_actionList.end();i++)
    {
        Action * action = (*i);
        delete action;
    }
    m_actionList.clear();
}

void ActionMgr::updateAction(Node *node, float dt)
{
    for(auto iter = m_actionList.begin();iter != m_actionList.end();)
    {
        Action * action = (*iter);
        action->update(node, dt);
        if(action->isDone())
        {
            iter = m_actionList.erase(iter);
            delete action;
        }else
        {
            iter ++;
        }
    }
}

} // namespace tzw
