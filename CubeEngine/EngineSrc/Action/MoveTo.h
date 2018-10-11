#ifndef TZW_MOVETO_H
#define TZW_MOVETO_H
#include "ActionInterval.h"
#include "../Math/vec3.h"

namespace tzw {

class MoveTo :public ActionInterval
{
public:
    MoveTo(float duration,vec3 from,vec3 to);
	void step(Node *node, float dt) override;
	void final(Node * node) override;
    virtual ~MoveTo();
private:
    vec3 m_fromPos;
    vec3 m_toPos;
};

} // namespace tzw

#endif // TZW_MOVETO_H
