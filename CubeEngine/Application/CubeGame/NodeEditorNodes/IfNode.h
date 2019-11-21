#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct IfNode: public BehaviorNode 
{
public:
	IfNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_leftValAttr;
	NodeAttr * m_rightValAttr;
	NodeAttr * m_cond;
};

}
