#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/GameNodeEditorNode.h"
namespace tzw {
struct ConstantIntNode : public GameNodeEditorNode
{
public:
	ConstantIntNode();
	void privateDraw() override;
	void load(rapidjson::Value& partData) override;
	void dump(rapidjson::Value& partDocObj,
	                rapidjson::Document::AllocatorType& allocator) override;
	int getNodeClass() override;
private:
	NodeAttr * m_attr;
};

}
