#include "test_OpenGL_event.h"
#include "test_OpenGL_main.h"
#include "runtime.h"
#include <cstdio>

static Runtime *g_sys;

enum KEYS
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    W,
    A,
    S,
    D
};

void getSys()
{
    if(g_sys == nullptr)
        g_sys = getRuntime();
}

void testOpenGLKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    getSys();
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_RIGHT:
                testOpenGLMainKeyboardPress(RIGHT);
                break;
            case GLFW_KEY_LEFT:
                testOpenGLMainKeyboardPress(LEFT);
                break;
            case GLFW_KEY_UP:
                testOpenGLMainKeyboardPress(UP);
                break;
            case GLFW_KEY_DOWN:
                testOpenGLMainKeyboardPress(DOWN);
                break;
            case GLFW_KEY_W:
                testOpenGLMainKeyboardPress(W);
                break;
            case GLFW_KEY_A:
                testOpenGLMainKeyboardPress(A);
                break;
            case GLFW_KEY_S:
                testOpenGLMainKeyboardPress(S);
                break;
            case GLFW_KEY_D:
                testOpenGLMainKeyboardPress(D);
                break;
            case GLFW_KEY_ESCAPE:
                g_sys->stop();
                break;
        }
    }
    else if(action == GLFW_RELEASE)
    {
        switch(key)
        {
            case GLFW_KEY_RIGHT:
                testOpenGLMainKeyboardRelease(RIGHT);
                break;
            case GLFW_KEY_LEFT:
                testOpenGLMainKeyboardRelease(LEFT);
                break;
            case GLFW_KEY_UP:
                testOpenGLMainKeyboardRelease(UP);
                break;
            case GLFW_KEY_DOWN:
                testOpenGLMainKeyboardRelease(DOWN);
                break;
            case GLFW_KEY_W:
                testOpenGLMainKeyboardRelease(W);
                break;
            case GLFW_KEY_A:
                testOpenGLMainKeyboardRelease(A);
                break;
            case GLFW_KEY_S:
                testOpenGLMainKeyboardRelease(S);
                break;
            case GLFW_KEY_D:
                testOpenGLMainKeyboardRelease(D);
                break;
            case GLFW_KEY_ESCAPE:
                g_sys->stop();
                break;
        }
    }
}

void testOpenGLMousePositionChangedEvent(GLFWwindow* window, double x, double y)
{
    testOpenGLMainMouseMove(x, y);
}
