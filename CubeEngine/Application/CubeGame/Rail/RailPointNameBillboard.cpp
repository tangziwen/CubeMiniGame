#include "RailPointNameBillboard.h"

#include "2D/LabelNew.h"
#include "2D/WorldBillboardSprite.h"
#include "EngineSrc/Base/Node.h"

#include <algorithm>

namespace tzw {

namespace
{
constexpr float NameBillboardPaddingX = 8.0f;
constexpr float NameBillboardMinWidth = 76.0f;
constexpr float NameBillboardHeight = 24.0f;
constexpr unsigned int NameBillboardBackgroundPriority = 20;
constexpr unsigned int NameBillboardLabelPriority = 21;
}

void RailPointNameBillboard::sync(Node* visualRoot, const vec3& worldAnchor, const std::string& text,
	const vec4& color)
{
	ensureView(visualRoot);
	if (!m_background || !m_label)
	{
		return;
	}

	refreshText(text);
	m_background->setWorldAnchor(worldAnchor);
	m_background->setColor(color);
	m_background->setOwnerVisible(true);
	m_label->setIsVisible(true);
}

void RailPointNameBillboard::hide()
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

void RailPointNameBillboard::ensureView(Node* visualRoot)
{
	if (m_background || !visualRoot)
	{
		return;
	}

	m_background = WorldBillboardSprite::createWithColor(vec4::fromRGB(45, 55, 65, 215), m_size);
	m_background->setScreenOffset(vec2(0.0f, 0.0f));
	m_background->setTouchEnable(false);
	m_background->setIsSwallow(false);
	m_background->setGlobalPiority(NameBillboardBackgroundPriority);
	visualRoot->addChild(m_background, false);

	m_label = LabelNew::create("");
	m_label->setGlobalPiority(NameBillboardLabelPriority);
	m_background->addChild(m_label, false);
}

void RailPointNameBillboard::refreshText(const std::string& text)
{
	if (m_text == text || !m_background || !m_label)
	{
		return;
	}

	m_text = text;
	m_label->setString(m_text);
	const vec2 labelSize = m_label->getContentSize();
	m_size.x = std::max(NameBillboardMinWidth, labelSize.x + NameBillboardPaddingX * 2.0f);
	m_size.y = NameBillboardHeight;
	m_background->setContentSize(m_size);
	layout();
}

void RailPointNameBillboard::layout()
{
	if (!m_label)
	{
		return;
	}
	const vec2 labelSize = m_label->getContentSize();
	m_label->setPos2D((m_size.x - labelSize.x) * 0.5f, m_size.y * 0.5f + 6.0f);
}

} // namespace tzw
