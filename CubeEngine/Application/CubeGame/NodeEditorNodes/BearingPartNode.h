#pragma once
#include "2D/GUISystem.h"

#include "CubeGame/GameNodeEditorNode.h"
namespace tzw {
class BearPart;
struct BearingPartNode : public GameNodeEditorNode
{
public:
	BearingPartNode(BearPart * bear);
	void privateDraw() override;
	BearPart * getProxy();
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
private:
	BearPart * m_bear;
};

}
