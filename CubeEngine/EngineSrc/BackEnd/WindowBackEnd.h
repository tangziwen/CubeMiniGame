#ifndef TZW_WINDOWBACKEND_H
#define TZW_WINDOWBACKEND_H

#include "../Engine/EngineDef.h"
namespace tzw {
class WindowBackEnd
{
public:
	virtual ~WindowBackEnd() = default;
	WindowBackEnd();
	virtual void prepare(int width, int height, bool isFullScreen);
    virtual void run();
	virtual void setUnlimitedCursor(bool enable);
	virtual void getMousePos(double* posX, double* posY);
	virtual int getMouseButton(int button);
	virtual void setWinSize(int width, int height);
	virtual void setIsFullScreen(bool isFullScreen);
	virtual void changeScreenSetting(int w, int h, bool isFullScreen);
private:
};

} // namespace tzw

#endif // TZW_WINDOWBACKEND_H
