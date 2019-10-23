#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class LiftPart : public GamePart
	{
	public:
		LiftPart();
		Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		void liftUp(float val);
		void setEffectedIsland(std::string islandGroup);
	public:
		float m_liftHeight;
		void cook();
		GamePartType getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
		std::string m_effectedIslandGroup;
	};
}
