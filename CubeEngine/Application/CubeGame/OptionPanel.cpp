#include "OptionPanel.h"
// #include "imgui.h"

#include "Utility/log/Log.h"
#include "Engine/Engine.h"
#include "BuildingSystem.h"
#define NOMINMAX
#include "dirent.h"
#include "Base/TranslationMgr.h"
#include "Rendering/Renderer.h"


namespace tzw
{
	static char* items[] = { "640*480", "800*600", "1024*768", "1360*768", "1280*1024", "1600*900", "1920*1080"};
	OptionPanel::OptionPanel():m_loadOpen(true),
		m_saveOpen(false)
	{
	}

	void OptionPanel::open()
	{
		m_screenSizeItemCurrent = 0;
		//find resolution
		for(int i = 0; i < (sizeof(items) / sizeof(char *)); i++)
		{
            char tmp[128] = {0};
			strcpy(tmp, items[i]);
			char *ptrW = strtok(tmp, "*");
			auto w = atof(ptrW);
			char *ptrH = strtok(NULL, "*");
			auto h = atof(ptrH);
			auto size = Engine::shared()->winSize();
			if(int(w) == int(size.x) && int(h) == int(size.y)) 
			{
				m_screenSizeItemCurrent = i;
				m_screenSizeEngineCurrent = i;
				break;
			}
		}
		m_isFullScreen = Engine::shared()->isIsFullScreen();
		m_isOpenSSAO = Renderer::shared()->isSsaoEnable();
		m_isOpenBloom = Renderer::shared()->isBloomEnable();
		m_isOpenAA = Renderer::shared()->isAaEnable();
		m_isOpenFog = Renderer::shared()->isFogEnable();
		m_isOpenShadow = Renderer::shared()->isShadowEnable();
		
	}
	
	static char inputBuff[128];
	void OptionPanel::drawIMGUI(bool * isOpen)
	{
		ImGui::Begin(TRC(u8"设置面板"), isOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		
		ImGui::TextUnformatted(TRC(u8"是否全屏"));
		if(ImGui::RadioButton(TRC(u8"全屏化"), m_isFullScreen)) 
		{
			m_isFullScreen = true;
		}
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"窗口化"), !m_isFullScreen))
		{
			m_isFullScreen = false;
		}
		if(m_isFullScreen != Engine::shared()->isIsFullScreen())
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}

		ImGui::TextUnformatted(TRC(u8"分辨率"));
		if(m_screenSizeItemCurrent != m_screenSizeEngineCurrent)
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}

        if (ImGui::BeginCombo(TRC(u8"分辨率"), items[m_screenSizeItemCurrent], 0)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (m_screenSizeItemCurrent == n);
                if (ImGui::Selectable(items[n], is_selected))
                    m_screenSizeItemCurrent = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }

		ImGui::TextUnformatted(TRC(u8"SSAO:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##SSAO"), m_isOpenSSAO)) m_isOpenSSAO = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##SSAO"), !m_isOpenSSAO)) m_isOpenSSAO = false;
		if(m_isOpenSSAO != Renderer::shared()->isSsaoEnable())
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}

		ImGui::TextUnformatted(TRC(u8"Bloom"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##BLOOM"), m_isOpenBloom)) m_isOpenBloom = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##BLOOM"), !m_isOpenBloom)) m_isOpenBloom = false;
		if(m_isOpenBloom != Renderer::shared()->isBloomEnable())
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}

		ImGui::TextUnformatted(TRC(u8"阴影:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##SHADOW"), m_isOpenShadow)) m_isOpenShadow = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##SHADOW"), !m_isOpenShadow)) m_isOpenShadow = false;
		if(m_isOpenShadow != Renderer::shared()->isShadowEnable())
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}

		ImGui::TextUnformatted(TRC(u8"抗锯齿:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##AA"), m_isOpenAA)) m_isOpenAA = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##AA"), !m_isOpenAA)) m_isOpenAA = false;
		if(m_isOpenAA != Renderer::shared()->isAaEnable())
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}


		ImGui::TextUnformatted(TRC(u8"雾效:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##FOG"), m_isOpenFog)) m_isOpenFog = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##FOG"), !m_isOpenFog)) m_isOpenFog = false;
		if(m_isOpenFog != Renderer::shared()->isFogEnable())
		{
			ImGui::SameLine();
			ImGui::TextUnformatted("*");
		}

		if(ImGui::Button(TRC(u8"保存设置")))
		{
			//the cost of changing screenSize is heavy, we need detect if it is really changed
			if(m_screenSizeEngineCurrent != m_screenSizeItemCurrent || m_isFullScreen != Engine::shared()->isIsFullScreen())
			{
				*isOpen = false;
	            char tmp[128] = {0};
				strcpy(tmp, items[m_screenSizeItemCurrent]);
				char *ptrW = strtok(tmp, "*");
				auto w = atof(ptrW);
				char *ptrH = strtok(NULL, "*");
				auto h = atof(ptrH);
				Engine::shared()->changeScreenSetting(w, h, m_isFullScreen);
			}
			Renderer::shared()->setSsaoEnable(m_isOpenSSAO);
			Renderer::shared()->setBloomEnable(m_isOpenBloom);
			Renderer::shared()->setAaEnable(m_isOpenAA);
			Renderer::shared()->setFogEnable(m_isOpenFog);
			Renderer::shared()->setShadowEnable(m_isOpenShadow);

			Engine::shared()->saveConfig();
		}
		ImGui::SameLine();
		if(ImGui::Button(TRC(u8"返回")))
		{
			*isOpen = false;
		}
		ImGui::End();
	}
}
