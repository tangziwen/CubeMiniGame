#pragma once
#include "../BehaviorNode.h"

namespace tzw {

struct VectorNode: public BehaviorNode 
{
public:
	VectorNode();
	NodeAttrValue execute() override;
	int getNodeClass() override;
protected:
	NodeAttr * m_composite;
	NodeAttr * m_out;
};

}
