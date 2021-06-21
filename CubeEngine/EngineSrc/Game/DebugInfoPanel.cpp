#include "DebugInfoPanel.h"
#include "../Engine/Engine.h"
#include "../Font/FontMgr.h"
#include "../Engine/EngineDef.h"
#include "Scene/SceneMgr.h"

#include "2d/GUISystem.h"
#include "BackEnd/RenderBackEnd.h"
#define PANEL_WIDTH 220
#define PANEL_HEIGHT 180

#define LINE_SPACE 22

#define MARGIN_Y 25
#define MARGIN_X 5

#define LABEL_FPS 0
#define LABEL_DRAWCALLS 1
#define LABEL_LOGIC_UPDATE 2
#define LABEL_APPLYRENDER 3
#define LABEL_VERTICES_COUNT 4
#define LABEL_INDICES_COUNT 5
#define LABEL_MEMORY_USAGE 6



namespace tzw {
static float values[30] = { 60.0f };
static float lastCalculateTime = 0.0f;

DebugInfoPanel::DebugInfoPanel()
{
	// init FPS
	for (int i = 0; i < IM_ARRAYSIZE(values); i++)
	{
		values[i] = 60.0f;
	}

	currFPS = 60.0f;
	drawCall = 0;
	logicUpdateTime = 0;
	renderUpdateTime = 0;
	verticesCount = 0;
	sceneCurrNodes = 0;
	//GUISystem::shared()->addObject(this);

}

void DebugInfoPanel::drawIMGUI(bool * isOpen)
{
	ImGui::Begin("Profiler", isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	static float f = 0.0f;
	updateInfo();
	ImGui::Text("Scene Nodes Amount: %d", sceneCurrNodes);
	ImGui::Text("Draw Call: %d", drawCall);
	ImGui::Text("logicUpdate: %d ms", logicUpdateTime);
	ImGui::Text("applyRender: %d ms", renderUpdateTime);
	ImGui::Text("indices: %d", verticesCount);
	static int values_offset = 0;
	
	char buff[100];
	snprintf(buff, sizeof(buff), "FPS %f", currFPS);
	ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, buff, 0.0f, 60.0f, ImVec2(0, 80));
	ImGui::End();
}

void DebugInfoPanel::setInfo()
{
}

void DebugInfoPanel::updateInfo()
{
	auto currentScene = g_GetCurrScene();
	lastCalculateTime += ImGui::GetIO().DeltaTime;
	if (lastCalculateTime > 0.25f)
	{
		currFPS = Engine::shared()->FPS();
		drawCall = Engine::shared()->getDrawCallCount();
		logicUpdateTime = Engine::shared()->getLogicUpdateTime();
		renderUpdateTime = Engine::shared()->getApplyRenderTime();
		verticesCount = Engine::shared()->getIndicesCount();
		for (int i = 0; i < IM_ARRAYSIZE(values) - 1; i++)
		{
			values[i] = values[i + 1];
		}
		values[IM_ARRAYSIZE(values) - 1] = currFPS;
		lastCalculateTime = 0.0f;
	}
}

} // namespace tzw
