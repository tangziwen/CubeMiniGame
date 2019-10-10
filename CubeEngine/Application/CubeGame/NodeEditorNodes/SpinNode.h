#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct SpinNode: public BehaviorNode 
{
public:
	SpinNode();
	void execute(GameNodeEditorNode * from) override;
protected:
	NodeAttr * m_bearingAttr;
	NodeAttr * m_signalAttr;

};

}
