#pragma once

namespace tzw
{
	class AudioEvent
	{
	public:
		AudioEvent();
		void pause(bool isPause);
		bool getIsPause();
		void setVolume(float volumeValue);
		int getHandler();
		void setHandler(int newHandler);
		void stop();
    private:
		int m_handler;	
	};
}
