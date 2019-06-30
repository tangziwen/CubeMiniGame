#ifndef TZW_WINDOWBACKEND_H
#define TZW_WINDOWBACKEND_H

#include "../Engine/EngineDef.h"
namespace tzw {
class WindowBackEnd
{
public:
	virtual ~WindowBackEnd() = default;
	WindowBackEnd();
	virtual void prepare(int width, int height);
    virtual void run();
	virtual void setUnlimitedCursor(bool enable);
	virtual void getMousePos(double* posX, double* posY);
	virtual int getMouseButton(int button);
private:
    TZW_SINGLETON_DECL(WindowBackEnd)
};

} // namespace tzw

#endif // TZW_WINDOWBACKEND_H
