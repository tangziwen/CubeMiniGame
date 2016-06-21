#include "ActionInterval.h"

namespace tzw {

ActionInterval::ActionInterval()
    :m_duration(1.0f),m_currentTime(0.0f)
{

}

void ActionInterval::update(Node *node, float dt)
{
    if(m_currentTime<=m_duration)
    {
        step(node,dt);
    }
    else
    {
        final(node);
    }
    m_currentTime += dt;
}

void ActionInterval::step(Node *node, float dt)
{
}

float ActionInterval::duration() const
{
    return m_duration;
}

void ActionInterval::setDuration(float duration)
{
    m_duration = duration;
}

bool ActionInterval::isDone()
{
    if(m_currentTime <= m_duration)
    {
        return false;
    }else
    {
        return true;
    }
}

float ActionInterval::currentTime() const
{
    return m_currentTime;
}

void ActionInterval::setCurrentTime(float currentTime)
{
    m_currentTime = currentTime;
}

void ActionInterval::final(Node *node)
{

}

} // namespace tzw
