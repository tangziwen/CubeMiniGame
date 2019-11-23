#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct PrintNode: public BehaviorNode 
{
public:
	PrintNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_strAttrVal;
};

}
