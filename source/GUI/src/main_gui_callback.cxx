#include "main_gui_callback.h"

#include "gui_handler.h"
#include "settings.h"
#include "tools.h"
#include "simulation.h"

#include <3D_engine/3D_engine.h>

#include <string>

extern Runtime *g_sys;
extern GUIHandler *g_gui;
extern Renderer *g_ren;
extern SimulationRenderer *g_simRen;

enum EVENTS
{
    EVENT_BUTTON_CLICKED,
    EVENT_SLIDER_MOVED,
    EVENT_FRAMEWINDOW_CLOSED
};

void buttonClickCallback(const CEGUI::EventArgs &args)
{
    mainGUICallback(args, EVENT_BUTTON_CLICKED);
}

void sliderMoveCallback(const CEGUI::EventArgs &args)
{
    mainGUICallback(args, EVENT_SLIDER_MOVED);
}

void frameWindowCloseCallback(const CEGUI::EventArgs &args)
{
    mainGUICallback(args, EVENT_FRAMEWINDOW_CLOSED);
}

void mainGUICallback(const CEGUI::EventArgs &args, int type)
{
    CEGUI::Window *tmp = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    std::string name = tmp->getName().c_str();

    switch(type)
    {
        case EVENT_BUTTON_CLICKED:
        {
            CEGUI::PushButton *wnd = static_cast<CEGUI::PushButton*>(tmp);

            if(name == "but_settings")
            {
                g_gui->getWindow("fra_settings")->setVisible(true);
            }
            else if(name == "root")
            {
                g_sys->toggleMouseControl(true);
                g_sys->toggleKeyboardControl(true);
            }
            else if(name == "but_import")
            {
                std::vector<std::string> vec;
                getFileFromExplorer(vec);
                g_simRen->loadDataFromFile(vec[0]);
            }
            else if(name == "but_begin")
            {
                if(g_simRen->isStarted())
                {
                    g_simRen->stop();
                    wnd->setText("BEGIN SIMULATION");
                }
                else
                {
                    g_simRen->start();
                    wnd->setText("STOP SIMULATION");
                }
            }

            break;
        }
        case EVENT_SLIDER_MOVED:
        {
            CEGUI::Slider *wnd = static_cast<CEGUI::Slider*>(tmp);
            float pos = wnd->getCurrentValue();

            if(name == "sli_fov")
            {
                g_ren->setFOV(pos / 0.5 * g_baseFOV * g_FOVModifier);
            }
            else if(name == "sli_mouse_h")
            {
                g_sys->setMouseSpeedHorizontal
                (
                    pos / 0.5 * g_baseMouseSpeedH * g_mouseSpeedHModifier
                );
            }
            else if(name == "sli_mouse_v")
            {
                g_sys->setMouseSpeedVertical
                (
                    pos / 0.5 * g_baseMouseSpeedV * g_mouseSpeedVModifier
                );
            }
            else if(name == "sli_speed")
            {
                g_sys->setKeyboardSpeed
                (
                    pos / 0.5 * g_baseKeyboardSpeed * g_keyboardSpeedModifier
                );
            }
            else if(name == "sli_timeline")
            {
                if(!g_simRen->isStarted())
                    g_simRen->setFrameToPosition(int(pos));
            }

            break;
        }
        case EVENT_FRAMEWINDOW_CLOSED:
        {
            CEGUI::FrameWindow *wnd = static_cast<CEGUI::FrameWindow*>(tmp);

            if(name == "fra_settings")
            {
                wnd->setVisible(false);
            }

            break;
        }
    }
}
