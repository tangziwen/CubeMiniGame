#include "BearPart.h"
#include "Interface/Drawable3D.h"

namespace tzw
{
BearPart::BearPart()
{
	m_a = nullptr;
	m_b = nullptr;
	m_isFlipped = false;
	m_node = nullptr;
}
void BearPart::updateFlipped()
{
	if(!m_node) return;
	if(m_isFlipped)
	{
		m_node->setColor(vec4(1.0, 0.0, 0.0, 1.0));
	}
	else
	{
		m_node->setColor(vec4(0.0, 1.0, 0.0, 1.0));
	}
}
}

