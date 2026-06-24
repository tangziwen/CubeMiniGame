#pragma once
#include "CubeGame/GameWorld.h"
#include "CubeGame/Rail/RailLine.h"
#include "2D/IMGUISystem.h"
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
			StationAdd,
			StationDelete,
			RoutePointAdd,
			RoutePointDelete,
			LineAddControlPoint,
			LineRemoveControlPoint,
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
		void drawParameterWindow(vec2 screenSize);
		void drawLineEditParameterPanel(RailSystem* railSystem);
		void handleEditorWorldInput();
		void setEditorState(EditorState state);
		bool isLineControlEditState() const;
		bool isTerrainState() const;
		bool isRailTrackState() const;
		bool isStationEditState() const;
		bool isRoutePointEditState() const;

		EditorState m_editorState = EditorState::None;
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
