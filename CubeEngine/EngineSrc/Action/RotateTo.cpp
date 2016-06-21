#include "RotateTo.h"
#include "../Base/Node.h"
namespace tzw {

RotateTo::RotateTo(float duration, vec3 from, vec3 to)
{
    setDuration(duration);
    m_fromQ.fromEulerAngle(from);
    m_toQ.fromEulerAngle(to);
}

void RotateTo::step(Node *node, float dt)
{
    auto t = m_currentTime / m_duration;
    Quaternion current = Quaternion::slerp(m_fromQ,m_toQ,t);
    node->setRotateQ(current);
}

void RotateTo::final(Node *node)
{
    node->setRotateQ(m_toQ);
}

RotateTo::~RotateTo()
{

}

} // namespace tzw
