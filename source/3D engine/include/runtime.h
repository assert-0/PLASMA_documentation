#pragma once

#include <windows.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//"Update" callback type definition - when set, Update gets called every frame
typedef void (UpdateCallback)(double);

//Runtime declaration

class Runtime
{
public:
    Runtime();

    ~Runtime();

    //Window options and main loop

    bool createWindow(const char *name, int w, int h);

    void destroyWindow(){glfwDestroyWindow(wnd);}

    GLFWwindow *getGLFWWindow(){return wnd;}

    bool run();

    void getScreenDimensions(int &w, int &h);

    void setScreenDimensions(int w, int h);

    void windowCallback(GLFWwindow* window, int w, int h);

    void stop(){exit = true;};

    //Camera control and Vsync settings.

    void toggleVsync(bool enable);

    //Enables/disables movement of the camera using keyboard input.
    //The keybinds are hardcoded in this version.
    void toggleKeyboardControl(bool enable);

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    //Enables/disables rotation of the camera using mouse input.
    void toggleMouseControl(bool enable);

    void mouseCallback(GLFWwindow* window, double x, double y);

    void getMouseSpeedHorizontal(float *mouseSpeedHorizontal)
    {
        *mouseSpeedHorizontal = this->mouseSpeedHorizontal;
    }

    void setMouseSpeedHorizontal(float mouseSpeedHorizontal)
    {
        this->mouseSpeedHorizontal = mouseSpeedHorizontal;
    }

    void getMouseSpeedVertical(float *mouseSpeedVertical)
    {
        *mouseSpeedVertical = this->mouseSpeedVertical;
    }

    void setMouseSpeedVertical(float mouseSpeedVertical)
    {
        this->mouseSpeedVertical = mouseSpeedVertical;
    }

    void getKeyboardSpeed(float *keyboardSpeed)
    {
        *keyboardSpeed = this->keyboardSpeed;
    }

    void setKeyboardSpeed(float keyboardSpeed)
    {
        this->keyboardSpeed = keyboardSpeed;
    }

    void getBackgroundColor(float *r, float *g, float *b)
    {
        *r = backR;
        *g = backG;
        *b = backB;
    }

    void setBackgroundColor(float r, float g, float b);

    void setUpdateCallback(UpdateCallback *callback){updateCallback = callback;}

private:
    void update(double timeElapsed);

    void render();

    void init();

    void destroy();

    GLFWwindow *wnd;
    int w, h;
    bool exit;
    bool vsync;
    bool keyboard, mouse;
    float mouseSpeedHorizontal;
    float mouseSpeedVertical;
    float keyboardSpeed;
    float backR, backG, backB;
    UpdateCallback *updateCallback;
};

//Runtime getter
Runtime *getRuntime();
