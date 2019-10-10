#include "TriggerNode.h"


namespace tzw
{
	TriggerNode::TriggerNode()
	{
		addOut(u8"");
	}

	void TriggerNode::trigger()
	{
		
	}

	vec3 TriggerNode::getNodeColor()
	{
		return vec3(1.0, 0.0, 0.0);
	}
}
