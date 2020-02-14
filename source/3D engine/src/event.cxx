#include "event.h"
#include "runtime.h"
#include "test_OpenGL_event.h"
#include "debug_flags.h"

#include <cstdio>
#include <GLFW/glfw3.h>

//System handles

static EventHandler g_eve;
static Runtime *g_sys;

EventHandler *getEventHandler()
{
    return &g_eve;
}

//Static member initialization

KeyCallback *EventHandler::keyCallback = nullptr;
CharCallback *EventHandler::charCallback = nullptr;
MouseLeaveCallback *EventHandler::mouseLeaveCallback = nullptr;
MousePositionCallback *EventHandler::mousePositionCallback = nullptr;
MouseClickCallback *EventHandler::mouseClickCallback = nullptr;
MouseScrollCallback *EventHandler::mouseScrollCallback = nullptr;
WindowSizeCallback *EventHandler::windowSizeCallback = nullptr;

//EventHandler methods

void EventHandler::initEvents()
{
    g_sys = getRuntime();
    GLFWwindow *wnd = g_sys->getGLFWWindow();

    #if PROJECT_3D_TEST_OPENGL
        glfwSetKeyCallback(wnd, testOpenGLKeyEvent);
        glfwSetCursorPosCallback(wnd, testOpenGLMousePositionChangedEvent);
    #else
        glfwSetKeyCallback(wnd, keyEvent);
        glfwSetCharCallback(wnd, charEvent);
        glfwSetCursorEnterCallback(wnd, mouseLeaveEvent);
        glfwSetCursorPosCallback(wnd, mousePositionChangedEvent);
        glfwSetMouseButtonCallback(wnd, mouseClickEvent);
        glfwSetScrollCallback(wnd, mouseScrollEvent);
        glfwSetWindowSizeCallback(wnd, windowSizeChangeEvent);
    #endif // PROJECT_3D_TEST_OPENGL
}

void EventHandler::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        #if PROJECT_3D_EVENT_DEBUG
            printf("Key pressed: %d\n", key);
        #endif
        switch(key)
        {

        }
    }
    else if(action == GLFW_RELEASE)
    {
        #if PROJECT_3D_EVENT_DEBUG
            printf("Key released: %d\n", key);
        #endif
    }
    g_sys->keyboardCallback(window, key, scancode, action, mods);
    if(keyCallback != nullptr)
        keyCallback(window, key, scancode, action, mods);
}

void EventHandler::charEvent(GLFWwindow* window, unsigned int codepoint)
{
    #if PROJECT_3D_EVENT_DEBUG
        printf("Character: %d (%c)\n", codepoint, char(codepoint));
    #endif
    if(charCallback != nullptr)
        charCallback(window, codepoint);
}

void EventHandler::mouseLeaveEvent(GLFWwindow* window, int entered)
{
    #if PROJECT_3D_EVENT_DEBUG
        printf("Mouse left: %d\n", entered);
    #endif
    if(mouseLeaveCallback != nullptr)
        mouseLeaveCallback(window, entered);
}

void EventHandler::mousePositionChangedEvent(GLFWwindow* window, double x, double y)
{
    #if PROJECT_3D_EVENT_DEBUG
        printf("New mouse position: X:%f Y:%f\n", x, y);
    #endif
    g_sys->mouseCallback(window, x, y);
    if(mousePositionCallback != nullptr)
        mousePositionCallback(window, x, y);
}

void EventHandler::mouseClickEvent(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        #if PROJECT_3D_EVENT_DEBUG
            printf("Mouse button pressed: %d\n", button);
        #endif
    }
    else if(action == GLFW_RELEASE)
    {
        #if PROJECT_3D_EVENT_DEBUG
            printf("Mouse button released: %d\n", button);
        #endif
    }
    if(mouseClickCallback != nullptr)
        mouseClickCallback(window, button, action, mods);
}

void EventHandler::mouseScrollEvent(GLFWwindow* window, double x, double y)
{
    #if PROJECT_3D_EVENT_DEBUG
        printf("Mouse scroll: X:%f Y:%f\n", x, y);
    #endif
    if(mouseScrollCallback != nullptr)
        mouseScrollCallback(window, x, y);
}

void EventHandler::windowSizeChangeEvent(GLFWwindow* window, int w, int h)
{
    #if PROJECT_3D_EVENT_DEBUG
        printf("New window size: %d %d\n", w, h);
    #endif
    g_sys->setScreenDimensions(w, h);
    g_sys->windowCallback(window, w, h);
    if(windowSizeCallback != nullptr)
        windowSizeCallback(window, w, h);
}
