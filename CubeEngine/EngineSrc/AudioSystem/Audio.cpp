#include "Audio.h"
#include "AudioSystem.h"
#include "Soloud/include/soloud.h"
#include "Soloud/include/soloud_wav.h"
namespace tzw
{

	Audio::Audio()
	{
		m_ptr = 0;
	}

	void * Audio::getPtr()
	{
		return m_ptr;
	}

	void Audio::setPtr(void * ptr)
	{
		m_ptr = ptr;
	}

	AudioEvent * Audio::Play()
	{
		AudioEvent * event = new AudioEvent();
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		SoLoud::Wav * wav = static_cast<SoLoud::Wav *>(m_ptr);
		event->setHandler(soloud->play(*wav));
		return event;
	}

	AudioEvent Audio::playWithOutCare()
	{
		AudioEvent event;
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		SoLoud::Wav * wav = static_cast<SoLoud::Wav *>(m_ptr);
		event.setHandler(soloud->play(*wav));
		return event;
	}

	AudioEvent* Audio::play3D(vec3 position)
	{
		AudioEvent * event = new AudioEvent();
		auto soloud = static_cast<SoLoud::Soloud *>(AudioSystem::shared()->getPtr());
		SoLoud::Wav * wav = static_cast<SoLoud::Wav *>(m_ptr);
		event->setHandler(soloud->play3d(*wav,position.x, position.y, position.z));
		soloud->set3dSourceParameters(event->getHandler(), position.x, position.y, position.z);
		soloud->set3dSourceAttenuation(event->getHandler(), SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.2);
		return event;
	}

	Audio::~Audio()
	{

	}

	void Audio::setIsLooping(bool isLoop)
	{
		SoLoud::Wav * wav = static_cast<SoLoud::Wav *>(m_ptr);
		wav->setLooping(isLoop);
	}
}

