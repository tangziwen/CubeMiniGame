#pragma once
#include "BearPart.h"
#include "GamePart.h"
#include "Event/Event.h"

namespace tzw
{
	class Island;
	class BearPart;
	class ControlPart : public GamePart, public EventListener
	{
	public:
		ControlPart();
		Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		void addForwardBearing(BearPart * bearing);
		void addSidewardBearing(BearPart * bearing);


		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;

		bool getIsActivate() const;
		void setActivate(bool value);
		void onFrameUpdate(float delta) override;
		void handleBearings();
		virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
		void load(rapidjson::Value& partData) override;
		Attachment* getAttachment(int index) override;
		int getAttachmentCount() override;
		~ControlPart();
		GameNodeEditorNode * getGraphNode() const;
		void generateName();
	public:
		int m_forward;
		int m_side;
		int getType() override;
		Attachment * m_attachment[6];
		BearPart * m_bearPart[6];
		std::vector<BearPart * > m_forwardBearing;
		std::vector<BearPart *> m_sidewardBearing;
		bool m_isActivate;
		GameNodeEditorNode * m_graphNode;
	};
}
