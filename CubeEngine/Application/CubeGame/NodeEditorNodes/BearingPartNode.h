#pragma once
#include "2D/IMGUISystem.h"
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
	void syncName() override;
	std::string getResType() override;
private:
	BearPart * m_part;
};

}
