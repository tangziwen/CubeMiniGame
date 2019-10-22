#pragma once
#include "BearPart.h"
#include "GamePart.h"
#include "noise/module/perlin.h"

namespace tzw
{
	class Island;
	class ThrusterPart : public GamePart
	{
	public:
		ThrusterPart();
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		Attachment * getBottomAttachment() override;
		Attachment * getAttachment(int index) override;
		int getAttachmentCount() override;
		void generateName() override;
		int getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
		void toggle(int openSignal);
		void updateForce(float dt);
		GameNodeEditorNode * getGraphNode() const;
		virtual ~ThrusterPart();
    private:
		int m_isOpen;
		float m_topRadius, m_bottomRadius, m_height;
		float m_phase, m_scale, m_t;
		vec3 m_phaseV3;
		float m_dir_t;
		GameNodeEditorNode * m_graphNode;
		noise::module::Perlin flatNoise;
	};
}
