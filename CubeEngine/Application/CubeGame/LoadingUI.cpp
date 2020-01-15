#include "LoadingUI.h"

#include "EngineSrc/CubeEngine.h"
#include "CubeGame/GameWorld.h"
#include "Game/ConsolePanel.h"
#include "Event/EventMgr.h"
#include "Technique/MaterialPool.h"
#include "3D/Sky.h"
#include "Collision/PhysicsMgr.h"
#include "Utility/math/TbaseMath.h"
#include "BuildingSystem.h"
#include "2D/GUISystem.h"
#include "../../EngineSrc/2D/imnodes.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

#include "UIHelper.h"

#include "dirent.h"
#include "Shader/ShaderMgr.h"
#include "2D/imgui_internal.h"
#include "algorithm"
#include "ThrusterPart.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleInitVelocityModule.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitPosModule.h"
#include "VehicleBroswer.h"
#include "PartSurfaceMgr.h"
#include "Base/TranslationMgr.h"
#include "2D/imgui_markdown.h"
#include "Utility/file/Tfile.h"


namespace tzw {
	TZW_SINGLETON_IMPL(LoadingUI);
	static void exitNow(Button * btn)
	{
	    exit(0);
	}

	static void onOption(Button * btn)
	{

	}

	LoadingUI::LoadingUI():m_tipsInfo("now Loading")
	{
		GUISystem::shared()->addObject(this);
		hide();
	}

	void LoadingUI::show()
	{
		m_isVisible = true;
	}

	void LoadingUI::hide()
	{
		m_isVisible = false;
	}

	void LoadingUI::drawIMGUI()
	{
		auto io = ImGui::GetIO();
		if(m_isVisible)
		{
			
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12, 0.12, 0.125, 1));
			ImGui::Begin("LoadingLayer",nullptr, window_flags);
			//ImGui::RenderFrame(ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGui::GetColorU32(ImVec4(0.0, 0, 0.0, 1.0)));
			auto loadCaption = "Loading...";
			auto captionTextSize= ImGui::CalcTextSize(loadCaption);
			ImGui::SetCursorScreenPos(ImVec2(io.DisplaySize.x / 2.0 - captionTextSize.x / 2.0f, io.DisplaySize.y/2.0 - captionTextSize.y - 5));
			GUISystem::shared()->imguiUseLargeFont();
			ImGui::TextUnformatted(loadCaption);
			ImGui::PopFont();
			auto texSize = ImGui::CalcTextSize(m_tipsInfo.c_str());
			ImGui::SetCursorScreenPos(ImVec2(io.DisplaySize.x / 2.0 - texSize.x / 2.0f, io.DisplaySize.y - texSize.y - 5));
			ImGui::TextUnformatted(m_tipsInfo.c_str());
			ImGui::End();
			ImGui::PopStyleVar(1);
			ImGui::PopStyleColor(1);
		}
	}

	std::string LoadingUI::getTipsInfo() const
	{
		return m_tipsInfo;
	}

	void LoadingUI::setTipsInfo(const std::string& tipsInfo)
	{
		m_tipsInfo = tipsInfo;
	}
} // namespace tzw
