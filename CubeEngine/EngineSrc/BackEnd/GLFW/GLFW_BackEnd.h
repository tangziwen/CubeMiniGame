#ifndef TZW_GLFW_BACKEND_H
#define TZW_GLFW_BACKEND_H

#include "../WindowBackEnd.h"
class GLFWwindow;
namespace tzw {

class GLFW_BackEnd : public WindowBackEnd
{
public:
    void prepare() override;
    void run() override;
    GLFW_BackEnd();
	void setUnlimitedCursor(bool enable) override;
	void getMousePos(double* posX, double* posY) override;
	int getMouseButton(int buttonMode) override;
private:
    GLFWwindow * m_window;
    TZW_SINGLETON_DECL(GLFW_BackEnd)
};

} // namespace tzw

#endif // TZW_GLFW_BACKEND_H
