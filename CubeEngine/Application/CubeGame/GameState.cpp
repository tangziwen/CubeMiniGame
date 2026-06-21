#include "GameState.h"

namespace tzw
{

PlayerMode GameState::getPlayerMode() const
{
	return m_playerMode;
}

void GameState::setPlayerMode(PlayerMode mode)
{
	if (m_playerMode == mode)
		return;
	m_playerMode = mode;
	if (onPlayerModeChanged)
		onPlayerModeChanged(mode);
	for (auto& listener : m_playerModeChangedListeners)
	{
		if (listener)
			listener(mode);
	}
}

void GameState::togglePlayerMode()
{
	setPlayerMode((m_playerMode == PlayerMode::Gameplay) ? PlayerMode::Editor : PlayerMode::Gameplay);
}

void GameState::addPlayerModeChangedListener(std::function<void(PlayerMode)> listener)
{
	m_playerModeChangedListeners.push_back(listener);
}

} // namespace tzw
