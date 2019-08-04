#pragma once
#include "rapidjson/document.h"
#include "GamePart.h"
namespace tzw
{
class vec3;
class Drawable3D;
class BlockPart;
struct Attachment;
class PhysicsHingeConstraint;
class GameConstraint : public GamePart
{
public:
	GameConstraint();
	Attachment * m_a;
	Attachment * m_b;
	virtual void enablePhysics(bool isEnable);
	virtual void dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator);
	bool isConstraint() override;
	void updateTransform(float dt);
protected:
	bool m_isEnablePhysics;

};
}
