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
	void init(Node * theNode) override;
	void step(Node *node, float dt) override;
	void final(Node * node) override;
    virtual ~RotateBy();
private:
    vec3 m_offset;
    vec3 m_originRotate;
};

} // namespace tzw

#endif // TZW_ROTATEBY_H
