#include "test_engine.h"
#include "runtime.h"
#include "renderer.h"
#include "event.h"
#include "object.h"

#include <vector>

#include <glm/glm.hpp>

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Vars and engine object handle getters.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool g_usingMouse = true;

static float g_backR = 0.1, g_backG = 0.1, g_backB = 0.15;

static float g_camX = 0.125f, g_camY = 0.0f, g_camZ = -10.0f;
static double g_horizontalAngle = 0.0, g_verticalAngle = 0.0;

static float g_FOV = glm::radians(90.0f);
static float g_maxRenderDistance = 1000000000.0f;

int g_numObjects = 4;
float g_colorR = 0.0f,
     g_colorG = 1.0f,
     g_colorB = 1.0f;
float g_objScale = 0.025;
float g_objGap = 0.25;

std::vector<int> g_objects;

static Runtime *g_sys;
static ObjectHandler *g_obj;
static ModelHandler *g_mod;
static Renderer *g_ren;
static EventHandler *g_eve;

static void acquireSystemHandle()
{
    g_sys = getRuntime();
}

static void acquireObjectHandle()
{
    g_obj = getObjectHandler();
}

static void acquireModelHandle()
{
    g_mod = getModelHandler();
}

static void acquireRendererHandle()
{
    g_ren = getRenderer();
}

static void acquireEventHandle()
{
    g_eve = getEventHandler();
}

static bool g_initialized;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Camera handling.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void setupCamera()
{
    g_ren->setCameraAngles(g_verticalAngle, g_horizontalAngle);
    g_ren->setCameraPosition(g_camX, g_camY, g_camZ);
    g_ren->setFOV(g_FOV);
    g_ren->setMaxrenderDistance(g_maxRenderDistance);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Object handling.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void loadModel()
{
    g_mod->loadModel("models\\sphere\\red\\sphere.obj", "red_sphere");
    g_mod->loadModel("models\\test\\suzanne.obj", "monkey");
}

static void createObjects()
{
    for(int a = 0; a < g_numObjects / 2; a++)
    {
        g_objects.push_back(g_obj->createObject("red_sphere"));
        g_objects.push_back(g_obj->createObject("monkey"));
    }
}

static void initObjects()
{
    std::vector<Object> &ar = *g_obj->getObjects();
    Object *tmp;

    for(int a = 0; a < g_numObjects; a++)
    {
        tmp = &ar[a];
        tmp->r = g_colorR;
        tmp->g = g_colorG;
        tmp->b = g_colorB;
        tmp->scale = g_objScale;
        tmp->x = (float)a * g_objGap;
        tmp->visible = true;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Event handling.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void mouseClick(GLFWwindow *wnd, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch(button)
        {
            case GLFW_MOUSE_BUTTON_3:
                g_usingMouse = !g_usingMouse;
                g_sys->toggleMouseControl(g_usingMouse);
                break;
        }
    }
}

static void setupCallbacks()
{
    g_eve->setMouseClickCallback(mouseClick);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Test functions.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void moveCamera(double timeElapsed)
{
    g_camX += timeElapsed / 20.0;
    setupCamera();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Main and init.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

float vertexAr[] =
{
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 0.0
};

unsigned short indexAr[] =
{
    0, 1, 2
};

void printFPS(double timeElapsed)
{
    printf("%g\n", 1.0 / timeElapsed);
}

static void init()
{
    acquireSystemHandle();
    acquireObjectHandle();
    acquireModelHandle();
    acquireRendererHandle();
    acquireEventHandle();

    loadModel();
    printf("Model loaded!\n");
    createObjects();
    printf("Objects created!\n");
    initObjects();
    printf("Objects initialized!\n");

    setupCamera();
    setupCallbacks();

    g_sys->toggleKeyboardControl(true);
    g_sys->toggleMouseControl(g_usingMouse);

    g_sys->setMouseSpeedHorizontal(0.00004f);
    g_sys->setMouseSpeedVertical(0.00004f);

    g_sys->setBackgroundColor(g_backR, g_backG, g_backB);

    //In normal usage, this callback would become the user's
    //main function called every frame.
    //g_sys->setUpdateCallback(printFPS);

    g_initialized = true;
}

void testEngineMain(double timeElapsed)
{
    if(!g_initialized)
        init();
}
