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
		//FMOD::SYSTEM * SYS = (FMOD::SYSTEM *)AUDIOSYSTEM::SHARED()->GETPTR();
		//SYS->PLAYSOUND(FMOD_CHANNEL_REUSE,(FMOD::SOUND *)M_PTR, FALSE, (FMOD::CHANNEL **)&M_CHANNEL);
	}

	void Audio::pause()
	{
		//FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		//((FMOD::Channel *)m_channel)->setPaused(true);
	}

	void Audio::resume()
	{
		//FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		//((FMOD::Channel *)m_channel)->setPaused(false);
	}

	void Audio::stop()
	{
		//FMOD::System * sys = (FMOD::System *)AudioSystem::shared()->getPtr();
		//((FMOD::Channel *)m_channel)->stop();
	}

	Audio::~Audio()
	{
		stop();
	}

}

