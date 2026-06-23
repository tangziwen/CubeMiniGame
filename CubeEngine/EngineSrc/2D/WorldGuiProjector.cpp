#include "WorldGuiProjector.h"

#include "Engine/Engine.h"
#include "Scene/Scene.h"
#include "Scene/SceneMgr.h"

namespace tzw {

bool WorldGuiProjector::project(const vec3& worldPos, vec2& outGuiPos)
{
	Scene* scene = g_GetCurrScene();
	if (!scene || !scene->defaultCamera())
	{
		return false;
	}

	const vec3 screenPos = scene->defaultCamera()->worldToScreen(worldPos);
	outGuiPos = screenToGui(screenPos.xy());
	return true;
}

vec2 WorldGuiProjector::screenToGui(const vec2& screenPos)
{
	return vec2(screenPos.x, Engine::shared()->windowHeight() - screenPos.y);
}

} // namespace tzw
