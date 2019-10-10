#pragma once
#include "2D/GUISystem.h"

#include "CubeGame/ResNode.h"
namespace tzw {
class ControlPart;
struct ControlPartNode : public ResNode
{
public:
	ControlPartNode(ControlPart * part);
	void privateDraw() override;
	void onLinkOut(int startID, int endID, GameNodeEditorNode * other) override;
	void onRemoveLinkOut(int startID, int endID, GameNodeEditorNode * other) override;
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
	void syncName() override;
	GamePart* getProxy() override;
private:
	ControlPart * m_part;

};

}
