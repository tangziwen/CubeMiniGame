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
		void attachToFromOtherIsland(Attachment * attach, BearPart * bearing) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		void liftUp(float val);
		void setEffectedIsland(Island * island);
	public:
		float m_liftHeight;
		void cook();
		int getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
		Island * m_effectedIsland;
	};
}
