#include "runtime.h"
#include "event.h"
#include "renderer.h"
#include "debug_flags.h"
#include "test_OpenGL_main.h"
#include "test_engine.h"

#include <cstdio>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <windows.h>

//Enums and globals

const double PI = 3.141592653589793238463;
const double PI2 = 6.283185307179586476925;

enum MOVE_DIRECTIONS
{
    MOVE_FORWARD = 1,
    MOVE_BACKWARD = 2,
    MOVE_RIGHT = 4,
    MOVE_LEFT = 8,
    MOVE_UP = 16,
    MOVE_DOWN = 32
};

enum KEYS
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    W,
    A,
    S,
    D,
    SPACE,
    SHIFT
};

static int g_moveDirection;

//System handles

static Runtime g_runtime;
static Renderer *g_ren;
static ObjectHandler *g_obj;
static EventHandler *g_eve;

Runtime *getRuntime()
{
    return &g_runtime;
}

//Error callback

static void errorCallback(int id, const char *message)
{
    printf("GLFW error: %s\n", message);
}

//Runtime helper functions

static void handleKeyDown(int key)
{
    switch(key)
    {
        case UP:
            if(!(g_moveDirection & MOVE_FORWARD))
                g_moveDirection += MOVE_FORWARD;
            break;
        case DOWN:
            if(!(g_moveDirection & MOVE_BACKWARD))
                g_moveDirection += MOVE_BACKWARD;
            break;
        case RIGHT:
            if(!(g_moveDirection & MOVE_RIGHT))
                g_moveDirection += MOVE_RIGHT;
            break;
        case LEFT:
            if(!(g_moveDirection & MOVE_LEFT))
                g_moveDirection += MOVE_LEFT;
            break;
        case SPACE:
            if(!(g_moveDirection & MOVE_UP))
                g_moveDirection += MOVE_UP;
            break;
        case SHIFT:
            if(!(g_moveDirection & MOVE_DOWN))
                g_moveDirection += MOVE_DOWN;
            break;
        case W:
            if(!(g_moveDirection & MOVE_FORWARD))
                g_moveDirection += MOVE_FORWARD;
            break;
        case S:
            if(!(g_moveDirection & MOVE_BACKWARD))
                g_moveDirection += MOVE_BACKWARD;
            break;
        case D:
            if(!(g_moveDirection & MOVE_RIGHT))
                g_moveDirection += MOVE_RIGHT;
            break;
        case A:
            if(!(g_moveDirection & MOVE_LEFT))
                g_moveDirection += MOVE_LEFT;
            break;
    }
}

static void handleKeyUp(int key)
{
    switch(key)
    {
        case UP:
            if(g_moveDirection & MOVE_FORWARD) g_moveDirection -= MOVE_FORWARD;
            break;
        case DOWN:
            if(g_moveDirection & MOVE_BACKWARD) g_moveDirection -= MOVE_BACKWARD;
            break;
        case RIGHT:
            if(g_moveDirection & MOVE_RIGHT) g_moveDirection -= MOVE_RIGHT;
            break;
        case LEFT:
            if(g_moveDirection & MOVE_LEFT) g_moveDirection -= MOVE_LEFT;
            break;
        case SPACE:
            if(g_moveDirection & MOVE_UP) g_moveDirection -= MOVE_UP;
            break;
        case SHIFT:
            if(g_moveDirection & MOVE_DOWN) g_moveDirection -= MOVE_DOWN;
            break;
        case W:
            if(g_moveDirection & MOVE_FORWARD) g_moveDirection -= MOVE_FORWARD;
            break;
        case S:
            if(g_moveDirection & MOVE_BACKWARD) g_moveDirection -= MOVE_BACKWARD;
            break;
        case D:
            if(g_moveDirection & MOVE_RIGHT) g_moveDirection -= MOVE_RIGHT;
            break;
        case A:
            if(g_moveDirection & MOVE_LEFT) g_moveDirection -= MOVE_LEFT;
            break;
    }
}

static void getForwardAndRigthVectors
(
    glm::vec3 &forward,
    glm::vec3 &right,
    double horizontalAngle,
    double verticalAngle
)
{
    forward.x = cos(verticalAngle) * sin(horizontalAngle);
    forward.y = sin(verticalAngle);
    forward.z = cos(verticalAngle) * cos(horizontalAngle);

    right.x = sin(horizontalAngle - PI / 2.0);
    right.y = 0;
    right.z = cos(horizontalAngle - PI / 2.0);
}

static void moveCamera
(
    int direction,
    double ammount,
    double horizontalAngle,
    double verticalAngle
)
{
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    double shiftX;
    double shiftY;
    double shiftZ;

    float camX, camY, camZ;

    getForwardAndRigthVectors(forward, right, horizontalAngle, verticalAngle);

    forward = glm::normalize(forward);
    right = glm::normalize(right);
    up = glm::cross(forward, right);
    up = glm::normalize(up);

    switch(direction)
    {
        case MOVE_FORWARD:
            forward *= 1.0;
            right *= 0.0;
            up *= 0.0;
            break;
        case MOVE_BACKWARD:
            forward *= -1.0;
            right *= 0.0;
            up *= 0.0;
            break;
        case MOVE_LEFT:
            forward *= 0.0;
            right *= 0.25;
            up *= 0.0;
            break;
        case MOVE_RIGHT:
            forward *= 0.0;
            right *= -0.25;
            up *= 0.0;
            break;
        case MOVE_UP:
            forward *= 0.0;
            right *= 0.0;
            up *= 0.25;
            break;
        case MOVE_DOWN:
            forward *= 0.0;
            right *= 0.0;
            up *= -0.25;
            break;
    }

    shiftX = (forward.x + right.x + up.x) * ammount;
    shiftY = (forward.y + right.y + up.y) * ammount;
    shiftZ = (forward.z + right.z + up.z) * ammount;

    g_ren->getCameraPosition(&camX, &camY, &camZ);

    camX += shiftX;
    camY += shiftY;
    camZ += shiftZ;

    g_ren->setCameraPosition(camX, camY, camZ);
}

static void heartbeat
(
    double timeElapsed,
    double cameraSpeed
)
{
    double cameraHorizontalAngle,
           cameraVerticalAngle;

    g_ren->getCameraAngles(&cameraVerticalAngle, &cameraHorizontalAngle);

    if(g_moveDirection & MOVE_FORWARD)
        moveCamera
        (
            MOVE_FORWARD,
            timeElapsed * cameraSpeed,
            cameraHorizontalAngle,
            cameraVerticalAngle
        );
    if(g_moveDirection & MOVE_BACKWARD)
        moveCamera
        (
            MOVE_BACKWARD,
            timeElapsed * cameraSpeed,
            cameraHorizontalAngle,
            cameraVerticalAngle
        );
    if(g_moveDirection & MOVE_RIGHT)
        moveCamera
        (
            MOVE_RIGHT,
            timeElapsed * cameraSpeed,
            cameraHorizontalAngle,
            cameraVerticalAngle
        );
    if(g_moveDirection & MOVE_LEFT)
        moveCamera
        (
            MOVE_LEFT,
            timeElapsed * cameraSpeed,
            cameraHorizontalAngle,
            cameraVerticalAngle
        );
    if(g_moveDirection & MOVE_UP)
        moveCamera
        (
            MOVE_UP,
            timeElapsed * cameraSpeed,
            cameraHorizontalAngle,
            cameraVerticalAngle
        );
    if(g_moveDirection & MOVE_DOWN)
        moveCamera
        (
            MOVE_DOWN,
            timeElapsed * cameraSpeed,
            cameraHorizontalAngle,
            cameraVerticalAngle
        );
}

//Runtime methods

void Runtime::init()
{
    g_ren = getRenderer();
    g_obj = getObjectHandler();
    g_eve = getEventHandler();
    glfwInit();
    ZeroMemory(this, sizeof(Runtime));
    toggleVsync(false);
    toggleMouseControl(false);
    toggleKeyboardControl(false);
    setMouseSpeedHorizontal(0.1);
    setMouseSpeedVertical(0.1);
    setKeyboardSpeed(1.0);
}

void Runtime::destroy()
{
    glfwTerminate();
}

Runtime::Runtime()
{
    init();
}

Runtime::~Runtime()
{
    destroy();
}

//Window options and main loop

bool Runtime::createWindow(const char *name, int w, int h)
{
    if(wnd != nullptr)
        return false;

    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    wnd = glfwCreateWindow(w, h, name, NULL, NULL);

    this->w = w;
    this->h = h;

    glfwMakeContextCurrent(wnd);
    glfwSwapInterval(0);    //No Vsync!

    glewExperimental = true;
    if(glewInit() != GLEW_OK)
        return false;

    glClearColor(0.0, 0.0, 0.2, 0.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glfwSetErrorCallback(errorCallback);

    g_eve->initEvents();

    g_ren->initShaders();

    return true;
}

bool Runtime::run()
{
    double currentTime, lastTime;
    lastTime = glfwGetTime();

    exit = false;

    while(glfwWindowShouldClose(wnd) == 0 && !(exit))
    {
        currentTime = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        #if PROJECT_3D_TEST_OPENGL
            testOpenGLMain(currentTime - lastTime);
        #endif // PROJECT_3D_TEST_OPENGL

        #if PROJECT_3D_TEST_ENGINE
            testEngineMain(currentTime - lastTime);
        #endif // PROJECT_3D_TEST_ENGINE
        render();
        update(currentTime - lastTime);

        lastTime = currentTime;

        glfwSwapBuffers(wnd);
        glfwPollEvents();
    }

    return true;
}

void Runtime::getScreenDimensions(int &w, int &h)
{
    w = this->w;
    h = this->h;
}

void Runtime::setScreenDimensions(int w, int h)
{
    this->w = w;
    this->h = h;
}

void Runtime::windowCallback(GLFWwindow* window, int w, int h)
{
    setScreenDimensions(w, h);
    glViewport(0, 0, w, h);
    g_ren->recalculateMatrices();
}

void Runtime::render()
{
    g_ren->renderMultipleObjects(&g_obj->getObjects()->front(), g_obj->getObjects()->size());
}

void Runtime::update(double timeElapsed)
{
    #if PROJECT_3D_RUNTIME_DEBUG
        printf("%lf\n", 1.0 / timeElapsed);
    #endif // PROJECT_3D_RUNTIME_DEBUG

    heartbeat
    (
        timeElapsed,
        keyboardSpeed
    );

    if(updateCallback != nullptr)
        updateCallback(timeElapsed);
}

//Camera control and Vsync settings.

void Runtime::toggleVsync(bool enable)
{
    if(vsync == enable)
        return;
    vsync = enable;
    if(vsync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
}

void Runtime::toggleKeyboardControl(bool enable)
{
    if(keyboard == enable)
        return;
    keyboard = enable;
}

void Runtime::keyboardCallback
(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    if(!keyboard)
        return;
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_RIGHT:
                handleKeyDown(RIGHT);
                break;
            case GLFW_KEY_LEFT:
                handleKeyDown(LEFT);
                break;
            case GLFW_KEY_UP:
                handleKeyDown(UP);
                break;
            case GLFW_KEY_DOWN:
                handleKeyDown(DOWN);
                break;
            case GLFW_KEY_W:
                handleKeyDown(W);
                break;
            case GLFW_KEY_A:
                handleKeyDown(A);
                break;
            case GLFW_KEY_S:
                handleKeyDown(S);
                break;
            case GLFW_KEY_D:
                handleKeyDown(D);
                break;
            case GLFW_KEY_SPACE:
                handleKeyDown(SPACE);
                break;
            case GLFW_KEY_LEFT_SHIFT:
                handleKeyDown(SHIFT);
                break;
        }
    }
    else if(action == GLFW_RELEASE)
    {
        switch(key)
        {
            case GLFW_KEY_RIGHT:
                handleKeyUp(RIGHT);
                break;
            case GLFW_KEY_LEFT:
                handleKeyUp(LEFT);
                break;
            case GLFW_KEY_UP:
                handleKeyUp(UP);
                break;
            case GLFW_KEY_DOWN:
                handleKeyUp(DOWN);
                break;
            case GLFW_KEY_W:
                handleKeyUp(W);
                break;
            case GLFW_KEY_A:
                handleKeyUp(A);
                break;
            case GLFW_KEY_S:
                handleKeyUp(S);
                break;
            case GLFW_KEY_D:
                handleKeyUp(D);
                break;
            case GLFW_KEY_SPACE:
                handleKeyUp(SPACE);
                break;
            case GLFW_KEY_LEFT_SHIFT:
                handleKeyUp(SHIFT);
                break;
        }
    }
}

void Runtime::toggleMouseControl(bool enable)
{
    if(mouse == enable)
        return;
    mouse = enable;
    if(mouse)
        glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Runtime::mouseCallback(GLFWwindow* window, double x, double y)
{
    if(!mouse)
        return;
    double dx = x - w / 2,
           dy = y - h / 2,
           camH,
           camV;

    glfwSetCursorPos
    (
        wnd,
        w / 2,
        h / 2
    );

    g_ren->getCameraAngles(&camV, &camH);

    camH += mouseSpeedHorizontal * dx;
    camV += mouseSpeedVertical * dy;

    if(fabs(camH) > PI2)
        camH -= floor(camH / PI2) * PI2;

    if(fabs(camV) > PI2)
        camV -= floor(camV / PI2) * PI2;

    g_ren->setCameraAngles(camV, camH);
}

void Runtime::setBackgroundColor(float r, float g, float b)
{
    backR = r;
    backG = g;
    backB = b;
    glClearColor(r, g, b, 0.0);
}
