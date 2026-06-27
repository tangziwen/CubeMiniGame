#include "EditorController.h"

#include "CubeGame/GameState.h"
#include "CubeGame/UI/InspectPanel.h"
#include "Event/Event.h"
#include "Event/EventMgr.h"

namespace tzw {

EditorState& EditorController::state()
{
	return m_state;
}

const EditorState& EditorController::state() const
{
	return m_state;
}

TerrainEditingController& EditorController::terrain()
{
	return m_terrainController;
}

RailEditingController& EditorController::rail()
{
	return m_railController;
}

void EditorController::switchToTab(int tabIndex, EditorModuleId moduleId)
{
	if (m_activeTab == tabIndex)
	{
		return;
	}

	if (m_state.module != moduleId)
	{
		m_state.clear();
	}
	m_activeTab = tabIndex;
}

void EditorController::onFrameUpdate(float)
{
	if (GameState::shared()->getPlayerMode() == PlayerMode::Editor)
	{
		m_railController.syncWorldVisuals(m_state);
		return;
	}

	m_railController.hideWorldVisuals();
}

bool EditorController::onMousePress(int button, InspectPanel& inspectPanel)
{
	if (GameState::shared()->getPlayerMode() != PlayerMode::Editor)
	{
		return false;
	}
	if (button == TZW_MOUSE_BUTTON_LEFT)
	{
		if (EventMgr::shared()->isMouseButtonConsumed(TZW_MOUSE_BUTTON_LEFT))
		{
			return false;
		}
		if (m_terrainController.handlePrimaryClick(m_state))
		{
			return true;
		}
		return m_railController.handlePrimaryClick(m_state, inspectPanel);
	}
	if (button == TZW_MOUSE_BUTTON_RIGHT)
	{
		return m_railController.handleSecondaryClick(m_state);
	}
	return false;
}

} // namespace tzw
