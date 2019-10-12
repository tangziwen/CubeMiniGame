#include "BehaviorNode.h"


namespace tzw
{
	BehaviorNode::BehaviorNode()
	{
		addInExe(u8"Ö´ÐÐ");
		addOutExe(u8"½ÓÐø");
	}

	NodeAttrValue BehaviorNode::execute()
	{
		return NodeAttrValue(nullptr);
	}

	vec3 BehaviorNode::getNodeColor()
	{
		return vec3(0.3, 0.3, 1.0);
	}

	int BehaviorNode::getType()
	{
		return Node_TYPE_BEHAVIOR;
	}
}
