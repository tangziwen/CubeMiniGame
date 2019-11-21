#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct EqualNode: public BehaviorNode 
{
public:
	EqualNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_leftValAttr;
	NodeAttr * m_rightValAttr;
	NodeAttr * m_out;
};

}
