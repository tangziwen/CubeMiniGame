#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct UseNode: public BehaviorNode 
{
public:
	UseNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_bearingAttr;

};

}
