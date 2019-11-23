#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class SwitchPart : public GamePart
	{
	public:
		explicit SwitchPart(std::string itemName);
		~SwitchPart();
		void generateName() override;
		GamePartType getType() override;
		BearPart * m_bearPart[6];
		void drawInspect() override;
		void use() override;
		bool isNeedDrawInspect() override;
    private:
		float m_topRadius, m_bottomRadius, m_height;
		GameNodeEditorNode * m_graphNode;
	};
}
