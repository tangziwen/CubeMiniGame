#include "DepthPolicy.h"
namespace tzw {
DepthPolicy::DepthPolicy()
{
    m_depthTest = RenderFlag::DepthTestMethod::Less;
    m_enable = true;
}

bool DepthPolicy::enable() const
{
    return m_enable;
}

void DepthPolicy::setEnable(bool enable)
{
    m_enable = enable;
}

RenderFlag::DepthTestMethod DepthPolicy::depthTest() const
{
    return m_depthTest;
}

void DepthPolicy::setDepthTest(const RenderFlag::DepthTestMethod &depthTest)
{
    m_depthTest = depthTest;
}

}
