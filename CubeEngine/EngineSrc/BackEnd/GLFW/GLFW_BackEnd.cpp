#include "GLFW_BackEnd.h"
#include "GL/glew.h"
#include "../RenderBackEnd.h"
#include "EngineSrc/BackEnd/AbstractDevice.h"
#include "External/TUtility/TUtility.h"
#include "../../Scene/SceneMgr.h"
#include <strstream>
#include "GLFW/glfw3.h"

namespace tzw {
TZW_SINGLETON_IMPL(GLFW_BackEnd)

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    AbstractDevice::shared()->mouseMoveEvent(vec2(xpos,ypos));
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    switch(action)
    {
    case GLFW_PRESS:
        AbstractDevice::shared()->mousePressEvent(button,vec2(x,y));
        break;
    case GLFW_RELEASE:
        AbstractDevice::shared()->mouseReleaseEvent(button,vec2(x,y));
        break;
    default:
        break;
    }
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::string theStr = "";
    switch(action)
    {
    case GLFW_PRESS:
    {
        AbstractDevice::shared()->keyPressEvent(key);
    }
        break;
    case GLFW_RELEASE:
    {
        AbstractDevice::shared()->keyReleaseEvent(key);
    }
        break;
    default:
        break;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void char_callback(GLFWwindow *, unsigned int theChar)
{
    AbstractDevice::shared()->charInputEvent(theChar);
}

void GLFW_BackEnd::prepare()
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
	int w = 1024;
	int h = 1024;
    glfwWindowHint(GLFW_RESIZABLE,0);
    m_window = glfwCreateWindow(w, h, EngineDef::versionStr, NULL, NULL);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCharCallback(m_window, char_callback);
	
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetCursorPosCallback(m_window, cursor_position_callback);
    glfwMakeContextCurrent(m_window);
    AbstractDevice::shared()->init(w, h);
}

void GLFW_BackEnd::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        AbstractDevice::shared()->update();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    glfwDestroyWindow(m_window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

GLFW_BackEnd::GLFW_BackEnd()
{

}

void GLFW_BackEnd::setUnlimitedCursor(bool enable)
{
	if (enable) 
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}


void GLFW_BackEnd::getMousePos(double* posX, double* posY)
{
	glfwGetCursorPos(m_window, posX, posY);
}

int GLFW_BackEnd::getMouseButton(int buttonMode)
{
	return glfwGetMouseButton(m_window, buttonMode);
}

} // namespace tzw
