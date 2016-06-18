#ifndef DEPTHPOLICY_H
#define DEPTHPOLICY_H

#include "../Rendering/RenderFlag.h"
namespace tzw {
class DepthPolicy
{
public:
    DepthPolicy();
    bool enable() const;
    void setEnable(bool enable);
    RenderFlag::DepthTestMethod depthTest() const;
    void setDepthTest(const RenderFlag::DepthTestMethod &depthTest);
protected:
    bool m_enable;
    RenderFlag::DepthTestMethod m_depthTest;
};
}
#endif // DEPTHPOLICY_H
