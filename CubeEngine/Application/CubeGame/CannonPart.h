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
		explicit CannonPart(std::string itemName);
		~CannonPart();
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		Attachment * getBottomAttachment() override;
		Attachment * getAttachment(int index) override;
		int getAttachmentCount() override;
		void generateName() override;
		GamePartType getType() override;
		BearPart * m_bearPart[6];
		void drawInspect() override;
		void use() override;
		bool isNeedDrawInspect() override;
    private:
		float m_topRadius, m_bottomRadius, m_height;
		GameNodeEditorNode * m_graphNode;
		float m_firingVelocity;
		float m_recoil;
	public:
		float getFiringVelocity() const;
		void setFiringVelocity(const float firingVelocity);
		float getRecoil() const;
		void setRecoil(const float recoil);
	};
}
