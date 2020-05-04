#ifndef TZW_AUDIO_SYSTEM_H
#define TZW_AUDIO_SYSTEM_H
#include <unordered_map>

#include "../Engine/EngineDef.h"

#include "Audio.h"
namespace tzw
{
	class AudioSystem : public Singleton<AudioSystem>
	{
	public:
		enum class DefaultOneShotSound
		{
			CLICK,
			CLINK,
			CLINK_REMOVE,
			DIGGING,
		};
		Audio * createSound(std::string filePath);
		void * getPtr();
		void update();
		AudioSystem();
		void init();
		void playOneShotSound(DefaultOneShotSound soundType);
	private:
		void     *m_system;
		std::unordered_map<DefaultOneShotSound, Audio *> m_defaultSoundPool;


	};
}
#endif
