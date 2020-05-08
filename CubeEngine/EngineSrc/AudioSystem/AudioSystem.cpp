#include "AudioSystem.h"
#include "Soloud/include/soloud.h"
#include "Soloud/include/soloud_wav.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw
{

	SoLoud::Soloud gSoloud; // SoLoud engine
	SoLoud::Wav gWave;      // One wave file



	Audio * AudioSystem::createSound(std::string filePath)
	{
		Audio * audio = new Audio();
		auto data  = Tfile::shared()->getData(filePath.c_str(), false);
		// Configure sound source
		auto wav = new SoLoud::Wav(); 
		wav->loadMem(data.getBytes(),data.getSize(),true);       // Load a wave file
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
		// gSoloud.set3dListenerAt();
		gSoloud.update3dAudio();
	}

	AudioSystem::AudioSystem()
	{

	}

	void AudioSystem::init()
	{
		auto result = gSoloud.init();
		printf("Audio BackEnd is %s %u\n", gSoloud.getBackendString(), result);

		auto event = createSound("Audio/windy_ambience.wav");
		event->setIsLooping(true);
		event->playWithOutCare().setVolume(0.45);

		auto ambient = createSound("Audio/ambient.wav");
		ambient->setIsLooping(true);
		ambient->playWithOutCare().setVolume(0.15);
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
					targetSrc = createSound("Audio/click.wav");
			}
				break;
			case DefaultOneShotSound::CLINK:
			{
					targetSrc = createSound("Audio/clink.wav");
			}
				break;
			case DefaultOneShotSound::CLINK_REMOVE:
			{
					targetSrc = createSound("Audio/clink2.wav");
			}
				break;
			case DefaultOneShotSound::DIGGING:
			{
					targetSrc = createSound("Audio/digging.wav");
			}
				break;
			case DefaultOneShotSound::SPRAY:
			{
					targetSrc = createSound("Audio/spray.wav");
			}
				break;
			case DefaultOneShotSound::ITEM_DROP:
			{
					targetSrc = createSound("Audio/item_drop.wav");
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

	void AudioSystem::setListenerParam(vec3 position,vec3 at, vec3 up)
	{
		
		gSoloud.set3dListenerParameters(position.x, position.y, position.z, at.x, at.y, at.z, up.x, up.y, up.z);
	}
}
