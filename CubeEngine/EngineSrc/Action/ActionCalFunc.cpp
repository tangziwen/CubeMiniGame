
#include "ActionCalFunc.h"

namespace tzw {

ActionCallFunc::ActionCallFunc(std::function<void ()> func)
    :m_duration(1.0f),m_currentTime(0.0f), m_func(func)
{

}

ActionCallFunc::ActionCallFunc(std::function<void ()> func, float duration)
	:m_duration(duration),m_currentTime(0.0f), m_func(func)
{
	
}

void ActionCallFunc::final(Node *node)
{
	if(m_func)
	{
		m_func();
	}
}

} // namespace tzw
