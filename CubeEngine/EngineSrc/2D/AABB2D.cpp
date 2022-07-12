#include "AABB2D.h"

tzw::AABB2D::AABB2D(vec2 minVal, vec2 maxVal)
	:m_min(minVal), m_max(maxVal)
{
}

tzw::AABB2D::AABB2D()
	:m_min(vec2(-99999.f, -99999.f)), m_max(vec2(99999.f, 99999.f))
{
}
