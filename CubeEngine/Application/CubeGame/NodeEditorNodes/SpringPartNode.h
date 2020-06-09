#pragma once
#include "2D/GUISystem.h"

#include "CubeGame/ResNode.h"
namespace tzw {
class SpringPart;
struct SpringPartNode : public ResNode
{
public:
	SpringPartNode(SpringPart * part);
	void privateDraw() override;
	void onLinkOut(int startID, int endID, GraphNode * other) override;
	virtual void load(rapidjson::Value& partData);
	void syncName() override;
	GamePart* getProxy() override;
	std::string getResType() override;
private:
	SpringPart * m_part;

};

}
