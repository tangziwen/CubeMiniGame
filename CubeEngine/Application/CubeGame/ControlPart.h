#pragma once
#include "BearPart.h"
#include "GamePart.h"
#include "AudioSystem/Audio.h"
#include "Event/Event.h"

namespace tzw
{
	class Island;
	class BearPart;
	class ControlPart : public GamePart, public EventListener
	{
	public:
		ControlPart();
		explicit ControlPart(std::string itemName);
		Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		bool getIsActivate() const;
		void setActivate(bool value);
		void onFrameUpdate(float delta) override;
		virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
		void load(rapidjson::Value& partData) override;
		Attachment* getAttachment(int index) override;
		virtual ~ControlPart();
		void generateName();
		void AddOnVehicle(Vehicle * vehicle) override;
	public:
		GamePartType getType() override;
		bool m_isActivate;
		AudioEvent* m_audioEvent;
	};
}
