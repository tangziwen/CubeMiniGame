#ifndef TZW_ROTATETO_H
#define TZW_ROTATETO_H

#include "ActionInterval.h"
#include "../Math/vec3.h"
#include "../Math/Quaternion.h"

namespace tzw {

class RotateTo :public ActionInterval
{
public:
    RotateTo(float duration,vec3 from,vec3 to);
    virtual void step(Node *node, float dt);
    virtual void final(Node * node);
    virtual ~RotateTo();
private:
    Quaternion m_fromQ,m_toQ;
};

} // namespace tzw

#endif // TZW_ROTATETO_H
