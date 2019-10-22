#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct ToggleNode: public BehaviorNode 
{
public:
	ToggleNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_bearingAttr;
	NodeAttr * m_signalAttr;

};

}
