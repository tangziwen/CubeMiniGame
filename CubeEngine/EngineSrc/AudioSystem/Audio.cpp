#include "Audio.h"
#include "AudioSystem.h"
#include "FMod/fmod.hpp"
namespace tzw
{

	Audio::Audio(std::string filePath)
	{
		m_ptr = 0;
		AudioSystem::shared()->createSound(filePath, this);
		m_channel = 0;
	}

	void * Audio::getPtr()
	{
		return m_ptr;
	}

	void Audio::setPtr(void * ptr)
	{
		m_ptr = ptr;
	}


	void Audio::Play()
	{
		FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		sys->playSound(FMOD_CHANNEL_REUSE,(FMOD::Sound *)m_ptr, false, (FMOD::Channel **)&m_channel);
	}

	void Audio::pause()
	{
		FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		((FMOD::Channel *)m_channel)->setPaused(true);
	}

	void Audio::resume()
	{
		FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		((FMOD::Channel *)m_channel)->setPaused(false);
	}

	void Audio::stop()
	{
		FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		((FMOD::Channel *)m_channel)->stop();
	}

	Audio::~Audio()
	{
		stop();
	}

}

