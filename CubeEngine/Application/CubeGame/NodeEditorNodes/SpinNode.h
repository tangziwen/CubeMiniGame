#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct SpinNode: public BehaviorNode 
{
public:
	SpinNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_bearingAttr;
	NodeAttr * m_signalAttr;

};

}
