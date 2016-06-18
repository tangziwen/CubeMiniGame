#define GLEW_STATIC
#include "GL/glew.h"
#include "BackEnd.h"
#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>
#include "../Engine/Engine.h"
#include "RenderBackEnd.h"
#ifndef QT_NO_OPENGL
#include "EngineSrc/BackEnd/QtGLWidget.h"
#endif
#include "External/TUtility/TUtility.h"
#include "../Scene/SceneMgr.h"


#include "GLFW/glfw3.h"
namespace tzw {

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int BackEnd::Run(int argc, char *argv[])
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    GLFWwindow* test_window = glfwCreateWindow(640, 480, "My GLFW Window", NULL, NULL);
    glfwMakeContextCurrent(test_window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(test_window, key_callback);
    glfwMakeContextCurrent(test_window);
    RenderBackEnd::shared()->initDevice();
    qDebug()<<reinterpret_cast<const char *>(glGetString(GL_VERSION));
//    while (!glfwWindowShouldClose(test_window))
//    {
//        float ratio;
//        int width, height;
//        glfwGetFramebufferSize(test_window, &width, &height);
//        ratio = width / (float) height;
//        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT);
//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
//        glBegin(GL_TRIANGLES);
//        glColor3f(1.f, 0.f, 0.f);
//        glVertex3f(-0.6f, -0.4f, 0.f);
//        glColor3f(0.f, 1.f, 0.f);
//        glVertex3f(0.6f, -0.4f, 0.f);
//        glColor3f(0.f, 0.f, 1.f);
//        glVertex3f(0.f, 0.6f, 0.f);
//        glEnd();
//        glfwSwapBuffers(test_window);
//        glfwPollEvents();
//    }
//    glfwDestroyWindow(test_window);
//    glfwTerminate();
//    exit(EXIT_SUCCESS);

    QApplication app(argc, argv);
    app.setApplicationName("CubeEngine");
    app.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL
    tzw::QtGLWidget widget;
    widget.show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
} // namespace tzw

