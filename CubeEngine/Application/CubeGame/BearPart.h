#pragma once


namespace tzw
{
class vec3;
class Drawable3D;
class BlockPart;
struct Attachment;
class PhysicsHingeConstraint;
class BearPart
{
public:
	BearPart();
	Attachment * m_a;
	Attachment * m_b;
	bool m_isFlipped;
	Drawable3D * m_node;
	void updateFlipped();
	PhysicsHingeConstraint * m_constrain;
	void enablePhysics(bool isEnable);
private:
	void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);

};
}
