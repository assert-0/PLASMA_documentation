#include "gui_handler.h"
#include "simulation.h"
#include "main_gui_callback.h"
#include "settings.h"
#include "tools.h"

#include <3D_engine/3D_engine.h>

#include <string>

Runtime *g_sys;
ObjectHandler *g_obj;
ModelHandler *g_mod;
EventHandler *g_eve;
Renderer *g_ren;
GUIHandler *g_gui;
SimulationEngine *g_sim;

void test()
{
    int UUID = g_obj->createObject("sphere");
    Object &obj = *g_obj->getObject(UUID);
    obj.r = 1.0;
    obj.g = 0.0;
    obj.b = 0.0;
    obj.scale = 0.5;
    obj.visible = true;
}

void getHandles()
{
    g_sys = getRuntime();
    g_obj = getObjectHandler();
    g_mod = getModelHandler();
    g_eve = getEventHandler();
    g_ren = getRenderer();
    g_gui = getGUIHandler();
    g_sim = getSimulationEngine();
}

void initGui()
{
    auto getPrefix =
    [](std::string in) -> std::string
    {
        std::string ret = "";
        auto res = in.find("_");

        if(res != std::string::npos)
            ret = in.substr(0, res);

        return ret;
    };

    auto setCallbacks =
    [&](CEGUI::Window *in)
    {
        auto type = getPrefix(in->getName().c_str());

        if(type != "")
        {
            if(type == "but")
                in->subscribeEvent
                (
                    CEGUI::PushButton::EventClicked,
                    buttonClickCallback
                );
            else if(type == "sli")
                in->subscribeEvent
                (
                    CEGUI::Slider::EventValueChanged,
                    sliderMoveCallback
                );
            else if(type == "fra")
                in->subscribeEvent
                (
                    CEGUI::FrameWindow::EventCloseClicked,
                    frameWindowCloseCallback
                );
        }
    };

    g_gui->initCEGUI();
    g_gui->executeForEachWindow(setCallbacks);

    g_gui->getWindow("root")->subscribeEvent
    (
        CEGUI::Window::EventMouseClick,
        buttonClickCallback
    );
}

void update(double timeElapsed)
{
    g_gui->update(timeElapsed);
    g_sim->update(timeElapsed);
}

int main()
{
    getHandles();

    g_sys->createWindow("PLASMA", 1280, 720);

    initGui();

    g_sys->setMouseSpeedHorizontal(g_baseMouseSpeedH);
    g_sys->setMouseSpeedVertical(g_baseMouseSpeedV);
    g_sys->setKeyboardSpeed(g_baseKeyboardSpeed);

    g_ren->setMaxrenderDistance(g_maxRenderDistance);
    g_ren->setCameraPosition(-10, -100, -800);

    g_sys->setUpdateCallback(update);

    g_sys->toggleVsync(true);

    g_sim->initSimulation();

    g_sys->run();
}
