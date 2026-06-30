#include "OptionPanel.h"
// #include "imgui.h"

#include "Utility/log/Log.h"
#include "Engine/Engine.h"
#include "BuildingSystem.h"
#define NOMINMAX
#include "dirent.h"
#include "Base/TranslationMgr.h"
#include "Rendering/GraphicsRenderer.h"


namespace tzw
{
	static char* items[] = { "640*480", "800*600", "1024*768", "1360*768", "1280*1024", "1600*900", "1920*1080"};
	static char* items_lang[] = { "Chinese", "English", "Chinese_Traditional"};
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
		//find language
		for(int i = 0; i < (sizeof(items_lang) / sizeof(char *)); i++)
		{
			if(TranslationMgr::shared()->getCurrLanguage() == items_lang[i])
			{
				m_langItemCurrent = i;
			}
		}
		m_isFullScreen = Engine::shared()->isIsFullScreen();
		auto& renderSettings = GraphicsRenderer::shared()->renderSettings();
		m_isOpenSSGI = renderSettings.ssgiEnabled();
		m_isOpenSSR = renderSettings.ssrEnabled();
		m_isOpenBloom = renderSettings.bloomEnabled();
		m_isOpenAA = renderSettings.aaEnabled();
		m_isOpenFog = renderSettings.fogEnabled();
		//m_isOpenShadow = Renderer::shared()->isShadowEnable();
		
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


        if (ImGui::BeginCombo(TRC(u8"语言设置"), items_lang[m_langItemCurrent], 0)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items_lang); n++)
            {
                bool is_selected = (m_langItemCurrent == n);
                if (ImGui::Selectable(items_lang[n], is_selected))
                    m_langItemCurrent = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }

		ImGui::TextUnformatted("SSGI:");
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##SSGI"), m_isOpenSSGI)) m_isOpenSSGI = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##SSGI"), !m_isOpenSSGI)) m_isOpenSSGI = false;

		ImGui::TextUnformatted("SSR:");
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##SSR"), m_isOpenSSR)) m_isOpenSSR = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##SSR"), !m_isOpenSSR)) m_isOpenSSR = false;

		ImGui::TextUnformatted(TRC(u8"Bloom"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##BLOOM"), m_isOpenBloom)) m_isOpenBloom = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##BLOOM"), !m_isOpenBloom)) m_isOpenBloom = false;


		ImGui::TextUnformatted(TRC(u8"阴影:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##SHADOW"), m_isOpenShadow)) m_isOpenShadow = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##SHADOW"), !m_isOpenShadow)) m_isOpenShadow = false;
	
		ImGui::TextUnformatted(TRC(u8"抗锯齿:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##AA"), m_isOpenAA)) m_isOpenAA = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##AA"), !m_isOpenAA)) m_isOpenAA = false;



		ImGui::TextUnformatted(TRC(u8"雾效:"));
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"开##FOG"), m_isOpenFog)) m_isOpenFog = true;
		ImGui::SameLine();
		if(ImGui::RadioButton(TRC(u8"关##FOG"), !m_isOpenFog)) m_isOpenFog = false;
		auto& renderSettings = GraphicsRenderer::shared()->renderSettings();
		renderSettings.setSSGIEnabled(m_isOpenSSGI);
		renderSettings.setSSREnabled(m_isOpenSSR);
		renderSettings.setBloomEnabled(m_isOpenBloom);
		renderSettings.setAAEnabled(m_isOpenAA);
		renderSettings.setFogEnabled(m_isOpenFog);

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


			TranslationMgr::shared()->load(items_lang[m_langItemCurrent]);
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
