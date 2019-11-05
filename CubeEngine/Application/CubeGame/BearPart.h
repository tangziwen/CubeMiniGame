#pragma once
#include "GameConstraint.h"
namespace tzw
{
class vec3;
class Drawable3D;
class BlockPart;
struct Attachment;
class PhysicsHingeConstraint;
	class GameNodeEditorNode;
class BearPart : public GameConstraint
{
public:
	BearPart();
	bool m_isFlipped;
	void updateFlipped();
	PhysicsHingeConstraint * m_constrain;
	void enablePhysics(bool isEnable) override;
	void dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator) override;
	Attachment* getFirstAttachment() override;
	Attachment * getAttachment(int index) override;
	int getAttachmentCount() override;
	virtual ~BearPart();
	GameNodeEditorNode * getGraphNode() const;
	void load(rapidjson::Value& partData) override;
	void setIsSteering(bool isSteering);
	bool getIsSteering() const;
	void setAngleLimit(bool isAngleLimit, float low, float high);
	void getAngleLimit(bool&isAngleLimit, float&low, float&high) const;
	void generateName() override;
	GamePartType getType() override;
	void enableAngularMotor(bool enableMotor, float targetVelocity, float maxMotorImpulse);
	void drawInspect() override;
	bool isNeedDrawInspect() override;
private:
	void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);
	GameNodeEditorNode * m_graphNode;
	bool m_isSteering;
	bool m_isAngleLimit;
	float m_angleLimitLow;
	float m_angleLimitHigh;

};
}
