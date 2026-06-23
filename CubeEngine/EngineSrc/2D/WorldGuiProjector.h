#ifndef TZW_WORLD_GUI_PROJECTOR_H
#define TZW_WORLD_GUI_PROJECTOR_H

#include "../Math/vec2.h"
#include "../Math/vec3.h"

namespace tzw {

class WorldGuiProjector
{
public:
	static bool project(const vec3& worldPos, vec2& outGuiPos);
	static vec2 screenToGui(const vec2& screenPos);
};

} // namespace tzw

#endif // TZW_WORLD_GUI_PROJECTOR_H
