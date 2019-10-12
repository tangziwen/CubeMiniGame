#include "TriggerNode.h"


namespace tzw
{
	TriggerNode::TriggerNode()
	{
	}

	void TriggerNode::trigger()
	{
		
	}

	vec3 TriggerNode::getNodeColor()
	{
		return vec3(1.0, 0.0, 0.0);
	}

	int TriggerNode::getType()
	{
		return Node_TYPE_TRIGGER;
	}
}
