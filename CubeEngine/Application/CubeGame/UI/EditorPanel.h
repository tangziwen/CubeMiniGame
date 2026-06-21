#pragma once
#include "CubeGame/GameWorld.h"
#include "CubeGame/Rail/RailLine.h"
#include "2D/GUISystem.h"
#include <functional>
namespace tzw
{
	class RailSystem;

	class EditorPanel : public IMGUIObject
	{
	public:
		enum class EditorState
		{
			None,
			CarveSphere,
			RaiseSphere,
			CarveBox,
			RaiseBox,
			PaintSphere,
			TrackAdd,
			TrackDelete,
			LineAddNode,
			LineRemoveNode,
		};

		void drawIMGUI(bool * isOpen) override;
		EditorPanel();
		std::function<void (std::string)> m_onCreate;
		void prepare();
    private:
		void applyTerrainEdit();
		void drawCameraModeWindow(vec2 screenSize);
		void drawCameraModeCombo();
		void drawTerrainTab();
		void drawTrainTab();
		void drawRailTrackPanel();
		void drawRailFloatingWindows();
		void drawRailLinePanel();
		void drawRailTrainPanel();
		void drawToolButton(const char* label, EditorState state, float width);
		void drawRailToolButton(const char* label, EditorState state, float width);
		void drawParameterPanel();
		void drawLineEditParameterPanel(RailSystem* railSystem);
		void handleEditorWorldInput();
		void setEditorState(EditorState state);
		bool isLineNodeEditState() const;
		bool isTerrainState() const;
		bool isRailTrackState() const;

		EditorState m_editorState = EditorState::CarveSphere;
		bool m_lineWindowOpen = false;
		bool m_trainWindowOpen = false;
		int m_activeTab = 0;
		float m_radius = 2.0f;
		float m_strength = 0.5f;
		float m_density = 0.5f;
		float m_boxHalfX = 2.0f;
		float m_boxHalfY = 2.0f;
		float m_boxHalfZ = 2.0f;
		int m_materialIndex = 0;
		int m_newTrainCarriageCount = 0;
	};
}
