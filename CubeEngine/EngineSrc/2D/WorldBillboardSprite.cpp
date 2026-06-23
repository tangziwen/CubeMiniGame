#include "WorldBillboardSprite.h"

#include "WorldGuiProjector.h"

namespace tzw {

WorldBillboardSprite::WorldBillboardSprite()
{
	setIsAccpectOcTtree(false);
	setTouchEnable(true);
	setIsSwallow(true);
}

WorldBillboardSprite::~WorldBillboardSprite()
{
	setTouchEnable(false);
}

WorldBillboardSprite* WorldBillboardSprite::createWithColor(vec4 color, vec2 contentSize)
{
	auto sprite = new WorldBillboardSprite();
	sprite->initWithColor(color, contentSize);
	return sprite;
}

void WorldBillboardSprite::setWorldAnchor(const vec3& worldAnchor)
{
	m_worldAnchor = worldAnchor;
}

void WorldBillboardSprite::setScreenOffset(const vec2& screenOffset)
{
	m_screenOffset = screenOffset;
}

void WorldBillboardSprite::setVisibleByOwner(bool isVisible)
{
	m_ownerVisible = isVisible;
	if (!m_ownerVisible)
	{
		Sprite::setIsVisible(false);
	}
}

bool WorldBillboardSprite::containsScreenPoint(vec2 pos)
{
	if (!m_ownerVisible || !getIsVisible())
	{
		return false;
	}

	const vec2 center = getPos2D();
	const vec2 halfSize = getContentSize() * 0.5f;
	return pos.x >= center.x - halfSize.x
		&& pos.x <= center.x + halfSize.x
		&& pos.y >= center.y - halfSize.y
		&& pos.y <= center.y + halfSize.y;
}

void WorldBillboardSprite::logicUpdate(float)
{
	if (!m_ownerVisible)
	{
		Sprite::setIsVisible(false);
		return;
	}

	vec2 guiPos;
	if (!WorldGuiProjector::project(m_worldAnchor, guiPos))
	{
		Sprite::setIsVisible(false);
		return;
	}

	setPos2D(guiPos + m_screenOffset);
	Sprite::setIsVisible(true);
}

bool WorldBillboardSprite::onMousePress(int, vec2 pos)
{
	if (!containsScreenPoint(pos))
	{
		return false;
	}

	m_isTouched = true;
	return true;
}

bool WorldBillboardSprite::onMouseRelease(int, vec2 pos)
{
	if (!m_ownerVisible || !getIsVisible())
	{
		m_isTouched = false;
		return false;
	}

	if (m_isTouched && containsScreenPoint(pos))
	{
		if (m_onBtnClicked)
		{
			m_onBtnClicked(this);
		}
		m_isTouched = false;
		return true;
	}

	m_isTouched = false;
	return false;
}

} // namespace tzw
