#ifndef TZW_AUDIO_H
#define TZW_AUDIO_H


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
		~Audio();
		void setIsLooping(bool isLoop);
	private:
		void * m_channel;
		void * m_ptr;
	};
}
#endif
