#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/GraphNode.h"
namespace tzw {
struct VarNode : public GraphNode
{
public:
	VarNode();
	void privateDraw() override;
	void load(rapidjson::Value& partData) override;
	void dump(rapidjson::Value& partDocObj,
	                rapidjson::Document::AllocatorType& allocator) override;
	int getNodeClass() override;
	void setInt(int value);
	void setFloat(float value);
	int getInt();
	float getFloat();
	std::string getVarName();
private:
	NodeAttr * m_attr;
	std::string m_varName;
};

}
