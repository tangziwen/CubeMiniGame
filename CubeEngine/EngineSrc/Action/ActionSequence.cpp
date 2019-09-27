#include "ActionSequence.h"

namespace tzw {

ActionSequence::ActionSequence(std::vector<Action *> &actionList)
    :m_duration(1.0f),m_currentTime(0.0f)
{
	for(auto i : actionList)
	{
		addAction(i);
	}
}

void ActionSequence::update(Node *node, float dt)
{
	if(!m_actionList.empty()) 
	{
		auto head = *m_actionList.begin();
		head->update(node, dt);
		if(head->isDone()) 
		{
			m_actionList.pop_front();
		}
	}
    m_currentTime += dt;
}

void ActionSequence::step(Node *node, float dt)
{
}

float ActionSequence::duration() const
{
    return m_duration;
}

void ActionSequence::setDuration(float duration)
{
    m_duration = duration;
}

bool ActionSequence::isDone()
{
	return m_actionList.empty();
}

float ActionSequence::currentTime() const
{
    return m_currentTime;
}

void ActionSequence::setCurrentTime(float currentTime)
{
    m_currentTime = currentTime;
}

void ActionSequence::final(Node *node)
{

}

void ActionSequence::addAction(Action* action)
{
	m_actionList.push_back(action);
}
} // namespace tzw
