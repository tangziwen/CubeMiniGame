#pragma once

#include "GameNodeEditorNode.h"
#include "GamePart.h"

namespace tzw {

struct TriggerNode: public GameNodeEditorNode 
{
public:
	TriggerNode();
	virtual void trigger();
	vec3 getNodeColor() override;
	int getType() override;
protected:

};

}
