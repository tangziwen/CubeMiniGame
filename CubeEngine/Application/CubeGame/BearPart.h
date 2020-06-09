#pragma once
#include "GameConstraint.h"
namespace tzw
{
class vec3;
class Drawable3D;
class BlockPart;
struct Attachment;
class PhysicsHingeConstraint;
class Physics6DOFConstraint;
struct GraphNode;
class BearPart : public GameConstraint
{
public:
	// BearPart();
	explicit BearPart(std::string itemName);
	bool m_isFlipped;
	void updateFlipped();
	PhysicsHingeConstraint * m_constrain;
	//Physics6DOFConstraint * m_constrain;
	void enablePhysics(bool isEnable) override;
	void dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator) override;
	Attachment* getFirstAttachment() override;
	Attachment * getAttachment(int index) override;
	int getAttachmentCount() override;
	virtual ~BearPart();
	GraphNode * getGraphNode() const;
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
	void updateConstraintState() override;
	void toggle(int openSignal) override;
	virtual void onUpdate(float dt);
private:
	GraphNode * m_graphNode;
	Material * m_xrayMat;
	bool m_isSteering;
	bool m_isAngleLimit;
	float m_angleLimitLow;
	float m_angleLimitHigh;

};
}
