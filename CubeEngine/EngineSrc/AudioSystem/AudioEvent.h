#pragma once
#include "Math/vec3.h"
namespace tzw {
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
  void set3DPosition(vec3 pos);

private:
  int m_handler;
};
}
