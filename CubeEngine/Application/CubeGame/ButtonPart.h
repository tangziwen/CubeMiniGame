#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class ButtonPart : public GamePart
	{
	public:
		explicit ButtonPart(std::string itemName);
		~ButtonPart();
		void generateName() override;
		GamePartType getType() override;
		BearPart * m_bearPart[6];
		void drawInspect() override;
		void use() override;
		bool isNeedDrawInspect() override;
		GameNodeEditorNode * getEditorNode() override;
		void onPressed();
		void onReleased();
    private:
		float m_topRadius, m_bottomRadius, m_height;
		GameNodeEditorNode * m_graphNode;
	};
}
