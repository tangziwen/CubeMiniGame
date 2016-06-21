#ifndef TZW_ACTIONINTERVAL_H
#define TZW_ACTIONINTERVAL_H
#include "Action.h"

namespace tzw {

class ActionInterval :public Action
{
public:
    ActionInterval();
    virtual void update(Node * node, float dt);
    virtual void step(Node * node,float dt);
    float duration() const;
    void setDuration(float duration);
    virtual bool isDone();
    float currentTime() const;
    void setCurrentTime(float currentTime);
    virtual void final(Node * node);
protected:
    float m_duration;
    float m_currentTime;
};

} // namespace tzw

#endif // TZW_ACTIONINTERVAL_H
