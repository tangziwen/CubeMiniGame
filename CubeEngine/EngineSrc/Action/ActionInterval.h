#ifndef TZW_ACTIONINTERVAL_H
#define TZW_ACTIONINTERVAL_H
#include "Action.h"

namespace tzw {

class ActionInterval :public Action
{
public:
    ActionInterval();
	explicit ActionInterval(float duration);
	void update(Node * node, float dt) override;
	void step(Node * node,float dt) override;
    float duration() const;
    void setDuration(float duration);
	bool isDone() override;
    float currentTime() const;
    void setCurrentTime(float currentTime);
	void final(Node * node) override;
protected:
    float m_duration;
    float m_currentTime;
};

} // namespace tzw

#endif // TZW_ACTIONINTERVAL_H
