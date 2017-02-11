#ifndef TZW_AUDIO_H
#define TZW_AUDIO_H


#include <string>
namespace tzw
{
	class Audio
	{
	public:
		Audio(std::string filePath);
		void * getPtr();
		void setPtr( void * ptr);
		void Play();
		void pause();
		void resume();
		void stop();
		~Audio();
	private:
		void * m_channel;
		void * m_ptr;
	};
}
#endif
