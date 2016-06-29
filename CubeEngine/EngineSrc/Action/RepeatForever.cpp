#include "RepeatForever.h"
#include <stdio.h>
namespace tzw {

RepeatForever::RepeatForever(ActionInterval *act)
{
    m_internalAct = act;
}

void RepeatForever::init(Node *theNode)
{
    m_internalAct->init(theNode);
}

void RepeatForever::update(Node *node, float dt)
{
    m_internalAct->update(node,dt);
    if(m_internalAct->isDone())
    {
        m_internalAct->init(node);
        m_internalAct->setCurrentTime(0.0f);
        m_internalAct->update(node,0.0f);
    }
}

bool RepeatForever::isDone()
{
    return false;
}

RepeatForever::~RepeatForever()
{

}

} // namespace tzw
