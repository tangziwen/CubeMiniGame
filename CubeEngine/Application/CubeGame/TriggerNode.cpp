#include "TriggerNode.h"
#include "BuildingSystem.h"

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

	void TriggerNode::handleKeyPress(int keyCode)
	{
	}

	void TriggerNode::handleKeyRelease(int keyCode)
	{
	}

	bool TriggerNode::isPlayerOnSeat()
	{
		auto controlPart = BuildingSystem::shared()->getCurrentControlPart();
		return controlPart && controlPart->getIsActivate() && controlPart->getVehicle()->getEditor() == getNodeEditor();
	}
}
