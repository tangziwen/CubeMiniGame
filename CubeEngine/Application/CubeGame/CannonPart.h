#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class CannonPart : public GamePart
	{
	public:
		CannonPart();
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		Attachment * getBottomAttachment() override;
		Attachment * getAttachment(int index) override;
		int getAttachmentCount() override;
		void generateName() override;
		GamePartType getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
		void use() override;
    private:
		float m_topRadius, m_bottomRadius, m_height;
		GameNodeEditorNode * m_graphNode;
	};
}
