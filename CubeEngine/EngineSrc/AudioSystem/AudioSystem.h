#ifndef TZW_AUDIO_SYSTEM_H
#define TZW_AUDIO_SYSTEM_H
#include "../Engine/EngineDef.h"

#include "Audio.h"
namespace tzw
{
	class AudioSystem : public Singleton<AudioSystem>
	{
	public:
		bool createSound(std::string filePath, Audio * audio);
		void * getPtr();
		void update();
		AudioSystem();
	private:
		void     *m_system;


	};
}
#endif
