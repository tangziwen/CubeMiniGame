#pragma once
#include "rapidjson/document.h"

namespace tzw
{
class vec3;
class Drawable3D;
class BlockPart;
struct Attachment;
class PhysicsHingeConstraint;
class SpringPart
{
public:
	SpringPart();
	Attachment * m_a;
	Attachment * m_b;
	Drawable3D * m_node;
	PhysicsHingeConstraint * m_constrain;
	void enablePhysics(bool isEnable);
	void dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator);
private:
	void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);

};
}
