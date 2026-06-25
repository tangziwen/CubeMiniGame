#ifndef TZW_WORLD_BILLBOARD_SPRITE_H
#define TZW_WORLD_BILLBOARD_SPRITE_H

#include "Sprite.h"

namespace tzw {

class WorldBillboardSprite : public Sprite
{
public:
	WorldBillboardSprite();
	~WorldBillboardSprite() override;
	static WorldBillboardSprite* createWithColor(vec4 color, vec2 contentSize);

	void setWorldAnchor(const vec3& worldAnchor);
	void setScreenOffset(const vec2& screenOffset);
	void setOwnerVisible(bool isVisible);
	bool isOwnerVisible() const;
	bool isProjectedVisible() const;
	bool containsScreenPoint(vec2 pos);
	void logicUpdate(float dt) override;
	bool onMousePress(int button, vec2 pos) override;
	bool onMouseRelease(int button, vec2 pos) override;

private:
	vec3 m_worldAnchor;
	vec2 m_screenOffset;
	bool m_ownerVisible = true;
	bool m_projectedVisible = false;
};

} // namespace tzw

#endif // TZW_WORLD_BILLBOARD_SPRITE_H
