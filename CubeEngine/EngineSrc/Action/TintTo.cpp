#include "TintTo.h"
#include "../Base/Node.h"
#include "Interface/Drawable.h"
#include <algorithm>

namespace tzw {

	TintTo::TintTo(float duration, vec4 fromColor, vec4 toColor)
	{
		setDuration(duration);
		m_fromColor = fromColor;
		m_toColor = toColor;
	}

	void TintTo::step(Node *node, float dt)
	{
		auto tmp = static_cast<Drawable *>(node);
		auto t = std::min( m_currentTime / m_duration, 1.0f);
		tmp->setColor(m_fromColor *(1 - t) + m_toColor * t);
	}

	void TintTo::final(Node *node)
	{
		auto tmp = static_cast<Drawable *>(node);
		tmp->setColor(m_toColor);
	}

	TintTo::~TintTo()
	{

	}

} // namespace tzw
