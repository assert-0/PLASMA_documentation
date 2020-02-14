#pragma once

#include <string>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

//Input callback type definitions

typedef void (InputKeyCallback)(GLFWwindow*, int, int, int, int);
typedef void (InputCharCallback)(GLFWwindow*, unsigned int);
typedef void (InputMouseLeaveCallback)(GLFWwindow*, int);
typedef void (InputMousePositionCallback)(GLFWwindow*, double, double);
typedef void (InputMouseClickCallback)(GLFWwindow*, int, int, int);
typedef void (InputMouseScrollCallback)(GLFWwindow*, double, double);
typedef void (InputWindowSizeCallback)(GLFWwindow*, int, int);

//GUI callback type definition

typedef void GUICallback(const CEGUI::WindowEventArgs&);

class GUIHandler
{
public:
    //Constructor and core methods

    GUIHandler();
    ~GUIHandler(){}

    void update(double timeElapsed);

    void initCEGUI();

    //Window methods

    CEGUI::Window *getWindow(std::string name);

    void findWindowAndExecute(std::string name, std::function<void(CEGUI::Window*)> func);

    void executeForEachWindow(std::function<void(CEGUI::Window*)> func);

    //Event callbacks

    static void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void charEvent(GLFWwindow* window, unsigned int codepoint);

    static void mouseLeaveEvent(GLFWwindow* window, int entered);

    static void mousePositionChangedEvent(GLFWwindow* window, double x, double y);

    static void mouseClickEvent(GLFWwindow* window, int button, int action, int mods);

    static void mouseScrollEvent(GLFWwindow* window, double x, double y);

    static void windowSizeChangeEvent(GLFWwindow* window, int w, int h);

    //Input callback setters

    void setKeyCallback(InputKeyCallback *callback){inputKeyCallback = callback;}

    void setCharCallback(InputCharCallback *callback){inputCharCallback = callback;}

    void setMouseLeaveCallback(InputMouseLeaveCallback *callback){inputMouseLeaveCallback = callback;}

    void setMousePositionCallback(InputMousePositionCallback *callback){inputMousePositionCallback = callback;}

    void setMouseClickCallback(InputMouseClickCallback *callback){inputMouseClickCallback = callback;}

    void setMouseScrollCallback(InputMouseScrollCallback *callback){inputMouseScrollCallback = callback;}

    void setWindowSizeCallback(InputWindowSizeCallback *callback){inputWindowSizeCallback = callback;}

private:
    //Callback pointers

    static InputKeyCallback *inputKeyCallback;
    static InputCharCallback *inputCharCallback;
    static InputMouseLeaveCallback *inputMouseLeaveCallback;
    static InputMousePositionCallback *inputMousePositionCallback;
    static InputMouseClickCallback *inputMouseClickCallback;
    static InputMouseScrollCallback *inputMouseScrollCallback;
    static InputWindowSizeCallback *inputWindowSizeCallback;
};

GUIHandler *getGUIHandler();
