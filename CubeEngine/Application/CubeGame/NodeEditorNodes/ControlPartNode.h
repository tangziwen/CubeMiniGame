#pragma once
#include "2D/GUISystem.h"

#include "CubeGame/GameNodeEditorNode.h"
namespace tzw {
class ControlPart;
struct ControlPartNode : public GameNodeEditorNode
{
public:
	ControlPartNode(ControlPart * part);
	void privateDraw() override;
	void onLinkOut(int startID, int endID, GameNodeEditorNode * other) override;
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
private:
	ControlPart * m_part;

};

}
