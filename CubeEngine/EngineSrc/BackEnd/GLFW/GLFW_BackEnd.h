#ifndef TZW_GLFW_BACKEND_H
#define TZW_GLFW_BACKEND_H

#include "../WindowBackEnd.h"
class GLFWwindow;
namespace tzw {

class GLFW_BackEnd : public WindowBackEnd
{
public:
    virtual void prepare();
    virtual void run();
    GLFW_BackEnd();
private:
    GLFWwindow * m_window;
    TZW_SINGLETON_DECL(GLFW_BackEnd)
};

} // namespace tzw

#endif // TZW_GLFW_BACKEND_H
