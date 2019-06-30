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
	void step(Node *node, float dt) override;
	void final(Node * node) override;
    virtual ~RotateTo();
private:
    Quaternion m_fromQ,m_toQ;
};

} // namespace tzw

#endif // TZW_ROTATETO_H
