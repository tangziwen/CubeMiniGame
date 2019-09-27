#pragma once
#include "Action.h"
#include "ActionInterval.h"
#include <vector>
#include <deque>

namespace tzw {

class ActionSequence :public ActionInterval
{
public:
    ActionSequence(std::vector<Action *> &actionList);
	void update(Node * node, float dt) override;
	void step(Node * node,float dt) override;
    float duration() const;
    void setDuration(float duration);
	bool isDone() override;
    float currentTime() const;
    void setCurrentTime(float currentTime);
    virtual void final(Node * node);
	void addAction(Action * action);
protected:
	std::deque<Action *> m_actionList;
    float m_duration;
    float m_currentTime;
};

} // namespace tzw
