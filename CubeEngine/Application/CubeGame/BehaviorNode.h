#pragma once

#include "GameNodeEditorNode.h"
#include "GamePart.h"

namespace tzw {

struct BehaviorNode: public GameNodeEditorNode 
{
public:
	BehaviorNode();
	NodeAttrValue execute() override;
	vec3 getNodeColor() override;
	int getType() override;
protected:

};

}
