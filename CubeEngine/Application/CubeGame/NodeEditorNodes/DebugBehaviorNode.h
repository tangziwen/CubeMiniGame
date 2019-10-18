#pragma once
#include "CubeGame/BehaviorNode.h"

namespace tzw {

struct DebugBehaviorNode: public BehaviorNode 
{
public:
	DebugBehaviorNode();
	NodeAttrValue execute() override;
};

}
