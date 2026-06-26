#ifndef TZW_RAIL_POINT_NAME_BILLBOARD_H
#define TZW_RAIL_POINT_NAME_BILLBOARD_H

#include "EngineSrc/Math/vec2.h"
#include "EngineSrc/Math/vec3.h"
#include "EngineSrc/Math/vec4.h"

#include <string>

namespace tzw {

class LabelNew;
class Node;
class WorldBillboardSprite;

class RailPointNameBillboard
{
public:
	void sync(Node* visualRoot, const vec3& worldAnchor, const std::string& text, const vec4& color);
	void hide();

private:
	void ensureView(Node* visualRoot);
	void refreshText(const std::string& text);
	void layout();

	WorldBillboardSprite* m_background = nullptr;
	LabelNew* m_label = nullptr;
	std::string m_text;
	vec2 m_size = vec2(80.0f, 24.0f);
};

} // namespace tzw

#endif // TZW_RAIL_POINT_NAME_BILLBOARD_H
