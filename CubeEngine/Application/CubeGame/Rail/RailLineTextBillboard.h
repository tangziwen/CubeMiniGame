#pragma once

#include "EngineSrc/Math/vec2.h"
#include "EngineSrc/Math/vec3.h"
#include "EngineSrc/Math/vec4.h"

#include <string>

namespace tzw {

class LabelNew;
class Node;
class WorldBillboardSprite;

class RailLineTextBillboard
{
public:
	void sync(Node* visualRoot, const vec3& worldAnchor, const std::string& text,
		const vec4& backgroundColor, const vec4& textColor, const vec2& minSize);
	void hide();

private:
	void ensureView(Node* visualRoot);
	void refreshText(const std::string& text, const vec2& minSize);
	void layout();

	WorldBillboardSprite* m_background = nullptr;
	LabelNew* m_label = nullptr;
	std::string m_text;
	vec2 m_size = vec2(26.0f, 22.0f);
};

} // namespace tzw
