#pragma once
#include "2D/GUISystem.h"

#include "CubeGame/GameNodeEditorNode.h"
namespace tzw {
class SpringPart;
struct SpringPartNode : public GameNodeEditorNode
{
public:
	SpringPartNode(SpringPart * part);
	void privateDraw() override;
	void onLinkOut(int startID, int endID, GameNodeEditorNode * other) override;
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
private:
	SpringPart * m_part;

};

}
