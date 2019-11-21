#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct AssignNode: public BehaviorNode 
{
public:
	AssignNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_leftValAttr;
	NodeAttr * m_rightValAttr;
};

}
