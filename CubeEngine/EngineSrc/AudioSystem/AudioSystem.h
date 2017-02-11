#ifndef TZW_AUDIO_SYSTEM_H
#define TZW_AUDIO_SYSTEM_H
#include "../Engine/EngineDef.h"

#include "Audio.h"
namespace tzw
{
	class AudioSystem
	{
		TZW_SINGLETON_DECL(AudioSystem)
	public:
		bool createSound(std::string filePath, Audio * audio);
		void * getPtr();
		void update();
	private:
		AudioSystem();
		void     *m_system;


	};
}
#endif
