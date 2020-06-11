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
		GraphNode * getEditorNode() override;
		void onPressed();
		void onReleased();
		void AddOnVehicle(Vehicle * vehicle) override;
    private:
		float m_topRadius, m_bottomRadius, m_height;
		GraphNode * m_graphNode;
	};
}
