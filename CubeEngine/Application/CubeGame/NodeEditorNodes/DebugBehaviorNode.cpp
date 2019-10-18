#include "DebugBehaviorNode.h"



namespace tzw
{
	DebugBehaviorNode::DebugBehaviorNode()
	{

	}

	NodeAttrValue DebugBehaviorNode::execute()
	{
		printf("test test test!!!!!\n");
		return NodeAttrValue();
	}

}
