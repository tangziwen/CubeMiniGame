#include "RailLineTextBillboard.h"

#include "2D/LabelNew.h"
#include "2D/WorldBillboardSprite.h"
#include "EngineSrc/Base/Node.h"

#include <algorithm>

namespace tzw {

namespace
{
constexpr float TextBillboardPaddingX = 7.0f;
}

void RailLineTextBillboard::sync(Node* visualRoot, const vec3& worldAnchor, const std::string& text,
	const vec4& backgroundColor, const vec4& textColor, const vec2& minSize)
{
	ensureView(visualRoot);
	if (!m_background || !m_label)
	{
		return;
	}

	refreshText(text, minSize);
	m_background->setWorldAnchor(worldAnchor);
	m_background->setColor(backgroundColor);
	m_background->setOwnerVisible(true);
	m_label->setColor(textColor);
	m_label->setIsVisible(true);
}

void RailLineTextBillboard::hide()
{
	if (m_background)
	{
		m_background->setOwnerVisible(false);
	}
	if (m_label)
	{
		m_label->setIsVisible(false);
	}
}

void RailLineTextBillboard::ensureView(Node* visualRoot)
{
	if (m_background || !visualRoot)
	{
		return;
	}

	m_background = WorldBillboardSprite::createWithColor(vec4::fromRGB(45, 55, 65, 230), m_size);
	m_background->setScreenOffset(vec2(0.0f, 0.0f));
	m_background->setTouchEnable(false);
	m_background->setIsSwallow(false);
	visualRoot->addChild(m_background);

	m_label = LabelNew::create("");
	m_background->addChild(m_label);
}

void RailLineTextBillboard::refreshText(const std::string& text, const vec2& minSize)
{
	if (!m_background || !m_label)
	{
		return;
	}

	if (m_text != text)
	{
		m_text = text;
		m_label->setString(m_text);
	}

	const vec2 labelSize = m_label->getContentSize();
	m_size.x = std::max(minSize.x, labelSize.x + TextBillboardPaddingX * 2.0f);
	m_size.y = minSize.y;
	m_background->setContentSize(m_size);
	layout();
}

void RailLineTextBillboard::layout()
{
	if (!m_label)
	{
		return;
	}
	const vec2 labelSize = m_label->getContentSize();
	m_label->setPos2D((m_size.x - labelSize.x) * 0.5f, m_size.y * 0.5f + 6.0f);
}

} // namespace tzw
