#ifndef TZW_AUDIO_H
#define TZW_AUDIO_H

#include "Math/vec3.h"
#include <string>
#include "AudioEvent.h"
namespace tzw
{
	class Audio
	{
	public:
		Audio();
		void * getPtr();
		void setPtr( void * ptr);
		AudioEvent * Play();
		AudioEvent playWithOutCare();
        AudioEvent* play3D(vec3 position);
		~Audio();
		void setIsLooping(bool isLoop);
	private:
		void * m_channel;
		void * m_ptr;
	};
}
#endif
