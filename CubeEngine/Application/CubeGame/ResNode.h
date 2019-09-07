#pragma once

#include "GameNodeEditorNode.h"
#include "GamePart.h"

namespace tzw {

struct ResNode: public GameNodeEditorNode 
{
public:
	ResNode();
	virtual void syncName();
	virtual GamePart * getProxy();
	virtual void handleNameEdit();
protected:

};

}
