#pragma once

#include "GameNodeEditorNode.h"
#include "GamePart.h"

namespace tzw {

struct BehaviorNode: public GameNodeEditorNode 
{
public:
	BehaviorNode();
	virtual void execute(GameNodeEditorNode * from);
	vec3 getNodeColor() override;
protected:

};

}
