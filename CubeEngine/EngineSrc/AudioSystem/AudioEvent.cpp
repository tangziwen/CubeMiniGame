#include "AudioEvent.h"
#include "AudioSystem.h"
#include "Soloud/include/soloud.h"
#include "Soloud/include/soloud_wav.h"
namespace tzw
{
	AudioEvent::AudioEvent():m_handler(0)
	{
	}

	void AudioEvent::pause(bool isPause)
	{
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		soloud->setPause(m_handler,isPause);
	}

	bool AudioEvent::getIsPause()
	{
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		return soloud->getPause(m_handler);
	}

	void AudioEvent::setVolume(float volumeValue)
	{
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		soloud->setVolume(m_handler,volumeValue);
	}

	int AudioEvent::getHandler()
	{
		return m_handler;
	}

	void AudioEvent::setHandler(int newHandler)
	{
		m_handler = newHandler;
	}

	void AudioEvent::stop()
	{
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		soloud->stop(m_handler);
	}
}

