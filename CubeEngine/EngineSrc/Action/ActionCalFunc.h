#pragma once
#include "Action.h"
#include "ActionInterval.h"
#include <functional>

namespace tzw {

class ActionCallFunc :public ActionInterval
{
public:
    ActionCallFunc(std::function<void ()> func);
	explicit ActionCallFunc(std::function<void ()> func, float duration);
	void final(Node * node) override;
	std::function<void ()> m_func;
protected:
    float m_duration;
    float m_currentTime;
};

} // namespace tzw
