#ifndef TZW_GLFW_BACKEND_H
#define TZW_GLFW_BACKEND_H

#include "../WindowBackEnd.h"
#define NOMINMAX

class GLFWwindow;
namespace tzw {

class GLFW_BackEnd : public WindowBackEnd
{
public:
    void prepare(int width, int height, bool isFullScreen) override;
    void run() override;
    GLFW_BackEnd();
	void setUnlimitedCursor(bool enable) override;
	void getMousePos(double* posX, double* posY) override;
	int getMouseButton(int buttonMode) override;
	void setWinSize(int width, int height) override;
	void setIsFullScreen(bool isFullScreen) override;
	void changeScreenSetting(int w, int h, bool isFullScreen) override;
private:
    GLFWwindow * m_window;
	bool glfwSetWindowCenter( GLFWwindow * window );
	float m_w,m_h;
};

} // namespace tzw

#endif // TZW_GLFW_BACKEND_H
