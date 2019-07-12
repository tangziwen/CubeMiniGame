#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class CylinderPart : public GamePart
	{
	public:
		CylinderPart();
		Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void attachToFromOtherIsland(Attachment * attach, BearPart * bearing) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
	public:
		void cook();
		int getType() override;
		Attachment * m_attachment[6];
		float m_topRadius, m_bottomRadius, m_height;
	};
}