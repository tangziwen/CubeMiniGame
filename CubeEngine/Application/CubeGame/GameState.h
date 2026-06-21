#pragma once
#include "EngineSrc/Base/Singleton.h"
#include <functional>
#include <vector>

namespace tzw
{

enum class PlayerMode
{
	Gameplay,
	Editor,
};

class GameState : public Singleton<GameState>
{
public:
	PlayerMode getPlayerMode() const;
	void setPlayerMode(PlayerMode mode);
	void togglePlayerMode();
	void addPlayerModeChangedListener(std::function<void(PlayerMode)> listener);

	std::function<void(PlayerMode)> onPlayerModeChanged;

private:
	PlayerMode m_playerMode = PlayerMode::Gameplay;
	std::vector<std::function<void(PlayerMode)>> m_playerModeChangedListeners;
};

} // namespace tzw
