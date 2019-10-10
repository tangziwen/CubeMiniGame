#include "BehaviorNode.h"


namespace tzw
{
	BehaviorNode::BehaviorNode()
	{
		addIn(u8"Ö´ÐÐ");
		addOut(u8"Ë³Ðò");
	}

	void BehaviorNode::execute(GameNodeEditorNode* from)
	{
		
	}

	vec3 BehaviorNode::getNodeColor()
	{
		return vec3(0.3, 0.3, 1.0);
	}
}
