#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/ResNode.h"
namespace tzw {
class BearPart;
struct BearingPartNode : public ResNode
{
public:
	BearingPartNode(BearPart * bear);
	void privateDraw() override;
	virtual GamePart * getProxy();
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
	void syncName() override;
private:
	BearPart * m_part;
};

}
