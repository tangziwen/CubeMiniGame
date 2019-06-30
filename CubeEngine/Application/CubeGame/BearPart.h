#pragma once


namespace tzw
{
class Drawable3D;
class BlockPart;
struct Attachment;
class BearPart
{
public:
	BearPart();
	Attachment * m_a;
	Attachment * m_b;
	bool m_isFlipped;
	Drawable3D * m_node;
	void updateFlipped();

};
}
