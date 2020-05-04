#include "AudioSystem.h"
#include "Soloud/include/soloud.h"
#include "Soloud/include/soloud_wav.h"
#include "Utility/log/Log.h"

namespace tzw
{

	SoLoud::Soloud gSoloud; // SoLoud engine
	SoLoud::Wav gWave;      // One wave file



	Audio * AudioSystem::createSound(std::string filePath)
	{
		Audio * audio = new Audio();
		// Configure sound source
		auto wav = new SoLoud::Wav(); 
		wav->load(filePath.c_str());       // Load a wave file
		audio->setPtr(wav);
		return audio;
	}

	void * AudioSystem::getPtr()
	{
		return &gSoloud;
	}

	void AudioSystem::update()
	{
		//FMOD::System * sys = (FMOD::System * )m_system;
		//sys->update();
	}

	AudioSystem::AudioSystem()
	{

	}

	void AudioSystem::init()
	{
		auto result = gSoloud.init();
		printf("backEnd %s %u\n", gSoloud.getBackendString(), result);
		// Configure sound source
		auto wav = new SoLoud::Wav(); 
		wav->load("./audio/windy_ambience.ogg");       // Load a wave file
		wav->setLooping(1);                          // Tell SoLoud to loop the sound
		int handle1 = gSoloud.play(*wav);             // Play it
		gSoloud.setVolume(handle1, 0.5f);            // Set volume; 1.0f is "normal"
		gSoloud.setPan(handle1, -0.2f);              // Set pan; -1 is left, 1 is right
		gSoloud.setRelativePlaySpeed(handle1, 0.9f); // Play a bit slower; 1.0f is normal
	}

	void AudioSystem::playOneShotSound(DefaultOneShotSound soundType)
	{
		Audio * targetSrc = nullptr;
		if(m_defaultSoundPool.find(soundType) == m_defaultSoundPool.end())
		{
			switch(soundType)
			{
			case DefaultOneShotSound::CLICK:
			{
					targetSrc = createSound("./audio/click.wav");
			}
				break;
			case DefaultOneShotSound::CLINK:
			{
					targetSrc = createSound("./audio/clink.wav");
			}
				break;
			case DefaultOneShotSound::CLINK_REMOVE:
			{
					targetSrc = createSound("./audio/clink2.wav");
			}
				break;
			case DefaultOneShotSound::DIGGING:
			{
					targetSrc = createSound("./audio/Digging.wav");
			}
				break;
			default: ;
			}
			if(targetSrc)
			{
				m_defaultSoundPool[soundType] = targetSrc;
			}
			
		}
		else
		{
			targetSrc = m_defaultSoundPool[soundType];
		}

		if(targetSrc)
		{
			targetSrc->playWithOutCare();
		}
	}
}
