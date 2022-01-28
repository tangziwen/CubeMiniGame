#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class PlanterPart : public GamePart
	{
	public:
		explicit PlanterPart(std::string itemName);
		~PlanterPart();
		void generateName() override;
		GamePartType getType() override;
		BearPart * m_bearPart[6];
		void drawInspect() override;
		void use() override;
		bool isNeedDrawInspect() override;
		void onPressed();
		void onReleased();
		void AddOnVehicle(Vehicle * vehicle) override;
    private:
		float m_topRadius, m_bottomRadius, m_height;
	};
}
