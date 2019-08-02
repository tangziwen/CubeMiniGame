#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class BlockPart : public GamePart
	{
	public:
		BlockPart();
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		Attachment * getBottomAttachment() override;
		Attachment * getAttachment(int index) override;
		int getAttachmentCount() override;
	public:
		void cook();
		int getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
	};
}
