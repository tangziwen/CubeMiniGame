#include "BearPart.h"
#include "Interface/Drawable3D.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Texture/TextureMgr.h"
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
	auto cylinder = static_cast<CylinderPrimitive *>(m_node);
	if(m_isFlipped)
	{
		cylinder->setTopBottomTex(TextureMgr::shared()->getByPath("Texture/bear_flipped.jpg"));
	}
	else
	{
		cylinder->setTopBottomTex(TextureMgr::shared()->getByPath("Texture/bear.jpg"));
	}
}
}

