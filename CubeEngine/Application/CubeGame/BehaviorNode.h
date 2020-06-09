#pragma once

#include "GraphNode.h"
#include "GamePart.h"

namespace tzw {

struct BehaviorNode: public GraphNode 
{
public:
	BehaviorNode();
	NodeAttrValue execute() override;
	vec3 getNodeColor() override;
	int getType() override;
	void handleExeOut() override;
protected:
	NodeAttr * m_out;
};

}
