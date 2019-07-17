#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class BearPart;
	class ControlPart : public GamePart
	{
	public:
		ControlPart();
		Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void attachToFromOtherIsland(Attachment * attach, BearPart * bearing) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		void addForwardBearing(BearPart * bearing);
		void addSidewardBearing(BearPart * bearing);

		void handleKeyPress(int keyCode);
		void handleKeyRelease(int keyCode);
	public:
		void cook();
		int getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
		std::vector<BearPart * > m_forwardBearing;
		std::vector<BearPart *> m_sidewardBearing;
	};
}
