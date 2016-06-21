#define GLEW_STATIC
#include "GL/glew.h"
#include "BackEnd.h"
#include "../Engine/Engine.h"
#include "RenderBackEnd.h"
#include "EngineSrc/BackEnd/QtGLWidget.h"
#include "External/TUtility/TUtility.h"
#include "../Scene/SceneMgr.h"
#include <strstream>
#include "GLFW/glfw3.h"

namespace tzw {

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}



static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    QtGLWidget::shared()->mouseMoveEvent(vec2(xpos,ypos));
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    switch(action)
    {
    case GLFW_PRESS:
        QtGLWidget::shared()->mousePressEvent(button,vec2(x,y));
        break;
    case GLFW_RELEASE:
        QtGLWidget::shared()->mouseReleaseEvent(button,vec2(x,y));
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
        std::strstream ss;
        ss <<  key;
        ss >> theStr;
        QtGLWidget::shared()->keyPressEvent(theStr);
    }
        break;
    case GLFW_RELEASE:
    {
        std::strstream ss;
        ss <<  key;
        ss >> theStr;
        QtGLWidget::shared()->keyReleaseEvent(theStr);
    }
        break;
    default:
        break;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int BackEnd::Run(int argc, char *argv[])
{
    QtGLWidget::shared();
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    GLFWwindow* mainWindow = glfwCreateWindow(1024, 768, "CubeEngine ", NULL, NULL);
    glfwMakeContextCurrent(mainWindow);
    glfwSwapInterval(1);
    glfwSetKeyCallback(mainWindow, key_callback);
    glfwSetMouseButtonCallback(mainWindow, mouse_button_callback);
    glfwSetCursorPosCallback(mainWindow, cursor_position_callback);
    glfwMakeContextCurrent(mainWindow);
    RenderBackEnd::shared()->initDevice();
    QtGLWidget::shared()->initializeGL(1024,768);
    while (!glfwWindowShouldClose(mainWindow))
    {
        QtGLWidget::shared()->paintGL();
        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
} // namespace tzw

