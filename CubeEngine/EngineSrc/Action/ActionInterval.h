#ifndef TZW_ACTIONINTERVAL_H
#define TZW_ACTIONINTERVAL_H
#include "Action.h"

namespace tzw {

class ActionInterval :public Action
{
public:
    ActionInterval();
	void update(Node * node, float dt) override;
	void step(Node * node,float dt) override;
    float duration() const;
    void setDuration(float duration);
	bool isDone() override;
    float currentTime() const;
    void setCurrentTime(float currentTime);
    virtual void final(Node * node);
protected:
    float m_duration;
    float m_currentTime;
};

} // namespace tzw

#endif // TZW_ACTIONINTERVAL_H
