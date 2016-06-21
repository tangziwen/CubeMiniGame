#ifndef TZW_ROTATEBY_H
#define TZW_ROTATEBY_H

#include "ActionInterval.h"
#include "../Math/vec3.h"
#include "../Math/Quaternion.h"

namespace tzw {

class RotateBy :public ActionInterval
{
public:
    RotateBy(float duration,vec3 offset);
    virtual void init(Node * theNode);
    virtual void step(Node *node, float dt);
    virtual void final(Node * node);
    virtual ~RotateBy();
private:
    vec3 m_offset;
    vec3 m_originRotate;
};

} // namespace tzw

#endif // TZW_ROTATEBY_H
