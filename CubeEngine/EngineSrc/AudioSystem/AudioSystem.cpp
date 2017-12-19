#include "AudioSystem.h"
#include "FMod/fmod.hpp"
namespace tzw
{

	bool AudioSystem::createSound(std::string filePath, Audio * audio)
	{
		FMOD::System * sys = (FMOD::System * )m_system;
		FMOD::Sound * ptr = 0;
		auto result = sys->createSound(filePath.c_str(), FMOD_DEFAULT, 0, &ptr);
		audio->setPtr(ptr);
		return result == FMOD_OK;
	}

	void * AudioSystem::getPtr()
	{
		return m_system;
	}

	void AudioSystem::update()
	{
		FMOD::System * sys = (FMOD::System * )m_system;
		sys->update();
	}

	AudioSystem::AudioSystem()
	{
		FMOD_RESULT result = FMOD::System_Create((FMOD::System **)&m_system);
		FMOD::System * sys = (FMOD::System *)m_system;
		sys->init(32, FMOD_INIT_NORMAL, 0);

	}


	TZW_SINGLETON_IMPL(AudioSystem)
}