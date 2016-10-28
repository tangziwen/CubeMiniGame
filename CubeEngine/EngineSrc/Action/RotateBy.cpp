#include "RotateBy.h"
#include "../Base/Node.h"
#include <stdio.h>
namespace tzw {

RotateBy::RotateBy(float duration, vec3 offset)
{
    setDuration(duration);
    m_offset = offset;
}

void RotateBy::init(Node *theNode)
{
    m_originRotate = theNode->getRotateE();
}

void RotateBy::step(Node *node, float dt)
{
    auto t = m_currentTime / m_duration;
    node->setRotateE( m_originRotate + m_offset * t);
}

void RotateBy::final(Node *node)
{
    node->setRotateE(m_originRotate + m_offset);
}

RotateBy::~RotateBy()
{

}

} // namespace tzw
