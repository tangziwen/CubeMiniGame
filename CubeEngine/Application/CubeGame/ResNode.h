#pragma once

#include "GraphNode.h"
#include "GamePart.h"

namespace tzw {

struct ResNode: public GraphNode 
{
public:
	ResNode();
	virtual void syncName();
	virtual GamePart * getProxy();
	virtual void handleNameEdit();
	vec3 getNodeColor() override;
	int getType() override;
	virtual std::string getResType();
	void dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator) override;
protected:

};

}
