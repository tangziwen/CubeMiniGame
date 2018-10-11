#ifndef TZW_REPEATFOREVER_H
#define TZW_REPEATFOREVER_H

#include "Action.h"
#include "ActionInterval.h"
namespace tzw {

class RepeatForever : public Action
{
public:
	explicit RepeatForever(ActionInterval * act);

	void init(Node * theNode) override;
	void update(Node * node, float dt) override;
	bool isDone() override;
    virtual ~RepeatForever();
private:
    ActionInterval * m_internalAct;
};

} // namespace tzw

#endif // TZW_REPEATFOREVER_H
