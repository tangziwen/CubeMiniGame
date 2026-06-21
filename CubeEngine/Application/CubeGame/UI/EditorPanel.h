#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>
namespace tzw
{
	class EditorPanel : public IMGUIObject
	{
	public:
		enum class ToolMode
		{
			CarveSphere,
			RaiseSphere,
			CarveBox,
			RaiseBox,
			PaintSphere,
		};

		void drawIMGUI(bool * isOpen) override;
		EditorPanel();
		std::function<void (std::string)> m_onCreate;
		void prepare();
    private:
		void applyTerrainEdit();
		void drawTerrainTab();
		void drawToolButton(const char* label, ToolMode mode, float width);
		void drawParameterPanel();

		ToolMode m_toolMode = ToolMode::CarveSphere;
		int m_activeTab = 0;
		float m_radius = 2.0f;
		float m_strength = 0.5f;
		float m_density = 0.5f;
		float m_boxHalfX = 2.0f;
		float m_boxHalfY = 2.0f;
		float m_boxHalfZ = 2.0f;
		int m_materialIndex = 0;
	};
}
