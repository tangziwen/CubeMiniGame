#include "MoveTo.h"
#include "../Base/Node.h"
namespace tzw {

MoveTo::MoveTo(float duration,vec3 from,vec3 to)
{
    setDuration(duration);
    m_fromPos = from;
    m_toPos = to;
}

void MoveTo::step(Node *node, float dt)
{
    auto t = m_currentTime / m_duration;
    auto resultPos = m_fromPos + (m_toPos - m_fromPos)*t;
    node->setPos(resultPos);
}

} // namespace tzw
