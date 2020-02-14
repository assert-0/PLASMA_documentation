#pragma once

#include "runtime.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Callback type definitions

typedef void (KeyCallback)(GLFWwindow*, int, int, int, int);
typedef void (CharCallback)(GLFWwindow*, unsigned int);
typedef void (MouseLeaveCallback)(GLFWwindow*, int);
typedef void (MousePositionCallback)(GLFWwindow*, double, double);
typedef void (MouseClickCallback)(GLFWwindow*, int, int, int);
typedef void (MouseScrollCallback)(GLFWwindow*, double, double);
typedef void (WindowSizeCallback)(GLFWwindow*, int, int);

//EventHandler declaration

class EventHandler
{
public:
    friend Runtime;

    //Event callbacks

    static void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void charEvent(GLFWwindow* window, unsigned int codepoint);

    static void mouseLeaveEvent(GLFWwindow* window, int entered);

    static void mousePositionChangedEvent(GLFWwindow* window, double x, double y);

    static void mouseClickEvent(GLFWwindow* window, int button, int action, int mods);

    static void mouseScrollEvent(GLFWwindow* window, double x, double y);

    static void windowSizeChangeEvent(GLFWwindow* window, int w, int h);

    //Callback setters

    void setKeyCallback(KeyCallback *callback){keyCallback = callback;}

    void setCharCallback(CharCallback *callback){charCallback = callback;}

    void setMouseLeaveCallback(MouseLeaveCallback *callback){mouseLeaveCallback = callback;}

    void setMousePositionCallback(MousePositionCallback *callback){mousePositionCallback = callback;}

    void setMouseClickCallback(MouseClickCallback *callback){mouseClickCallback = callback;}

    void setMouseScrollCallback(MouseScrollCallback *callback){mouseScrollCallback = callback;}

    void setWindowSizeCallback(WindowSizeCallback *callback){windowSizeCallback = callback;}

private:
    //Init function - gets called by runtime when it creates a window.
    void initEvents();

    //Callback pointers

    static KeyCallback *keyCallback;
    static CharCallback *charCallback;
    static MouseLeaveCallback *mouseLeaveCallback;
    static MousePositionCallback *mousePositionCallback;
    static MouseClickCallback *mouseClickCallback;
    static MouseScrollCallback *mouseScrollCallback;
    static WindowSizeCallback *windowSizeCallback;
};

//EventHandler getter
EventHandler *getEventHandler();
