#include "gui_handler.h"

#include <3D_engine/3D_engine.h>

//Globals

static CEGUI::DefaultResourceProvider *g_rp;
static CEGUI::OpenGL3Renderer *g_guiRen;
static CEGUI::GUIContext *g_con;
static CEGUI::Window *g_rootWindow;

//System handles

static GUIHandler g_guiHandler;
extern EventHandler *g_eve;
extern Runtime *g_sys;

GUIHandler *getGUIHandler(){return &g_guiHandler;}

//Static member initialization

InputKeyCallback *GUIHandler::inputKeyCallback = nullptr;
InputCharCallback *GUIHandler::inputCharCallback = nullptr;
InputMouseLeaveCallback *GUIHandler::inputMouseLeaveCallback = nullptr;
InputMousePositionCallback *GUIHandler::inputMousePositionCallback = nullptr;
InputMouseClickCallback *GUIHandler::inputMouseClickCallback = nullptr;
InputMouseScrollCallback *GUIHandler::inputMouseScrollCallback = nullptr;
InputWindowSizeCallback *GUIHandler::inputWindowSizeCallback = nullptr;

//GUIHandler helper functions

static CEGUI::Key::Scan glfwToCeguiKey(int glfwKey)
{
   switch (glfwKey)
   {
      case GLFW_KEY_UNKNOWN: return CEGUI::Key::Unknown;
      case GLFW_KEY_ESCAPE: return CEGUI::Key::Escape;
      case GLFW_KEY_F1: return CEGUI::Key::F1;
      case GLFW_KEY_F2: return CEGUI::Key::F2;
      case GLFW_KEY_F3: return CEGUI::Key::F3;
      case GLFW_KEY_F4: return CEGUI::Key::F4;
      case GLFW_KEY_F5: return CEGUI::Key::F5;
      case GLFW_KEY_F6: return CEGUI::Key::F6;
      case GLFW_KEY_F7: return CEGUI::Key::F7;
      case GLFW_KEY_F8: return CEGUI::Key::F8;
      case GLFW_KEY_F9: return CEGUI::Key::F9;
      case GLFW_KEY_F10: return CEGUI::Key::F10;
      case GLFW_KEY_F11: return CEGUI::Key::F11;
      case GLFW_KEY_F12: return CEGUI::Key::F12;
      case GLFW_KEY_F13: return CEGUI::Key::F13;
      case GLFW_KEY_F14: return CEGUI::Key::F14;
      case GLFW_KEY_F15: return CEGUI::Key::F15;
      case GLFW_KEY_UP: return CEGUI::Key::ArrowUp;
      case GLFW_KEY_DOWN: return CEGUI::Key::ArrowDown;
      case GLFW_KEY_LEFT: return CEGUI::Key::ArrowLeft;
      case GLFW_KEY_RIGHT: return CEGUI::Key::ArrowRight;
      case GLFW_KEY_LEFT_SHIFT: return CEGUI::Key::LeftShift;
      case GLFW_KEY_RIGHT_SHIFT: return CEGUI::Key::RightShift;
      case GLFW_KEY_LEFT_CONTROL: return CEGUI::Key::LeftControl;
      case GLFW_KEY_RIGHT_CONTROL: return CEGUI::Key::RightControl;
      case GLFW_KEY_LEFT_ALT: return CEGUI::Key::LeftAlt;
      case GLFW_KEY_RIGHT_ALT: return CEGUI::Key::RightAlt;
      case GLFW_KEY_TAB: return CEGUI::Key::Tab;
      case GLFW_KEY_ENTER: return CEGUI::Key::Return;
      case GLFW_KEY_BACKSPACE: return CEGUI::Key::Backspace;
      case GLFW_KEY_INSERT: return CEGUI::Key::Insert;
      case GLFW_KEY_DELETE: return CEGUI::Key::Delete;
      case GLFW_KEY_PAGE_UP: return CEGUI::Key::PageUp;
      case GLFW_KEY_PAGE_DOWN: return CEGUI::Key::PageDown;
      case GLFW_KEY_HOME: return CEGUI::Key::Home;
      case GLFW_KEY_END: return CEGUI::Key::End;
      case GLFW_KEY_NUM_LOCK: return CEGUI::Key::NumLock;
      case GLFW_KEY_KP_ENTER: return CEGUI::Key::NumpadEnter;

      default: return CEGUI::Key::Unknown;
   }
}

CEGUI::MouseButton glfwToCeguiButton(int button)
{
    switch(button)
    {
        case GLFW_MOUSE_BUTTON_LEFT: return CEGUI::LeftButton;
        case GLFW_MOUSE_BUTTON_RIGHT: return CEGUI::RightButton;
        case GLFW_MOUSE_BUTTON_MIDDLE: return CEGUI::MiddleButton;
        case GLFW_MOUSE_BUTTON_4: return CEGUI::X1Button;
        case GLFW_MOUSE_BUTTON_5: return CEGUI::X2Button;
        default: return CEGUI::NoButton;
    }
}

//------------------//
//GUIHandler methods//
//------------------//

//Constructor and core methods

GUIHandler::GUIHandler()
{
    g_eve->setCharCallback(charEvent);
    g_eve->setKeyCallback(keyEvent);
    g_eve->setMouseClickCallback(mouseClickEvent);
    g_eve->setMouseLeaveCallback(mouseLeaveEvent);
    g_eve->setMousePositionCallback(mousePositionChangedEvent);
    g_eve->setMouseScrollCallback(mouseScrollEvent);
    g_eve->setWindowSizeCallback(windowSizeChangeEvent);
}

void GUIHandler::update(double timeElapsed)
{
    glDisable(GL_DEPTH_TEST);

    CEGUI::System::getSingleton().renderAllGUIContexts();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    g_con->injectTimePulse(timeElapsed);
    CEGUI::System::getSingleton().injectTimePulse(timeElapsed);
}

void GUIHandler::initCEGUI()
{
    g_guiRen = &CEGUI::OpenGL3Renderer::bootstrapSystem();
    g_rp = static_cast<CEGUI::DefaultResourceProvider*>
    (
        CEGUI::System::getSingleton().getResourceProvider()
    );
    g_rp->setResourceGroupDirectory("animations", "./resources/GUI/animations/");
    g_rp->setResourceGroupDirectory("fonts", "./resources/GUI/fonts/");
    g_rp->setResourceGroupDirectory("imagesets", "./resources/GUI/imagesets/");
    g_rp->setResourceGroupDirectory("layouts", "./resources/GUI/layouts");
    g_rp->setResourceGroupDirectory("looknfeels", "./resources/GUI/looknfeels/");
    g_rp->setResourceGroupDirectory("lua_scripts", "./resources/GUI/lua_scripts/");
    g_rp->setResourceGroupDirectory("schemes", "./resources/GUI/schemes/");
    g_rp->setResourceGroupDirectory("schemas", "./resources/GUI/xml_schemas/");

    CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

    g_con = &CEGUI::System::getSingleton().createGUIContext
    (
        g_guiRen->getDefaultRenderTarget()
    );

    CEGUI::SchemeManager::getSingleton().createFromFile
    (
        "TaharezLook.scheme", "schemes"
    );
    CEGUI::SchemeManager::getSingleton().createFromFile
    (
        "GWEN.scheme", "schemes"
    );
    CEGUI::SchemeManager::getSingleton().createFromFile
    (
        "WindowsLook.scheme", "schemes"
    );

    CEGUI::FontManager::getSingleton().createFreeTypeFont
    (
        "Batang-12", 12, false, "batang.ttf", "fonts", CEGUI::ASM_Vertical
    );
    CEGUI::FontManager::getSingleton().createFreeTypeFont
    (
        "Jura-13", 10, false, "Jura-DemiBold.ttf", "fonts", CEGUI::ASM_Vertical
    );
    CEGUI::FontManager::getSingleton().createFreeTypeFont
    (
        "Jura-10", 7, false, "Jura-DemiBold.ttf", "fonts", CEGUI::ASM_Vertical
    );
    CEGUI::FontManager::getSingleton().createFreeTypeFont
    (
        "Jura-18", 15, false, "Jura-DemiBold.ttf", "fonts", CEGUI::ASM_Vertical
    );

    g_con->setDefaultFont("DejaVuSans-12");

    g_rootWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile
    (
        "window.layout"
    );

    g_con->setRootWindow(g_rootWindow);
}

//Window methods

CEGUI::Window *GUIHandler::getWindow(std::string name)
{
    auto res = g_con->getRootWindow()->getChildRecursive(name);
    if(res != nullptr)
        return res;
    return g_con->getRootWindow();
}

void GUIHandler::findWindowAndExecute
(
    std::string name,
    std::function<void(CEGUI::Window*)> func
)
{
    func(g_con->getRootWindow()->getChildRecursive(name));
}

void GUIHandler::executeForEachWindow
(
    std::function<void(CEGUI::Window*)> func
)
{
    std::function<void(CEGUI::Window*)> recursion =
    [&](CEGUI::Window *wnd) -> void
    {
        func(wnd);
        for(int a = 0; a < wnd->getChildCount(); a++)
            recursion(wnd->getChildAtIdx(a));
    };

    recursion(g_con->getRootWindow());
}

//Event callbacks

void GUIHandler::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        g_con->injectKeyDown(glfwToCeguiKey(key));
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                g_sys->stop();
                break;
        }
    }
    else if(action == GLFW_RELEASE)
    {
        g_con->injectKeyUp(glfwToCeguiKey(key));
        switch(key)
        {
            case GLFW_KEY_E:
                g_sys->toggleMouseControl(false);
                g_sys->toggleKeyboardControl(false);
                break;
            case GLFW_KEY_Q:
                if(g_con->getRootWindow()->isVisible())
                    g_con->getRootWindow()->setVisible(false);
                else
                    g_con->getRootWindow()->setVisible(true);
                break;
        }
    }
    if(inputKeyCallback != nullptr)
        inputKeyCallback(window, key, scancode, action, mods);
}

void GUIHandler::charEvent(GLFWwindow* window, unsigned int codepoint)
{
    g_con->injectChar(codepoint);
    if(inputCharCallback != nullptr)
        inputCharCallback(window, codepoint);
}

void GUIHandler::mouseLeaveEvent(GLFWwindow* window, int entered)
{
    g_con->injectMouseLeaves();
    if(inputMouseLeaveCallback != nullptr)
        inputMouseLeaveCallback(window, entered);
}

void GUIHandler::mousePositionChangedEvent(GLFWwindow* window, double x, double y)
{
    g_con->injectMousePosition(x, y);
    if(inputMousePositionCallback != nullptr)
        inputMousePositionCallback(window, x, y);
}

void GUIHandler::mouseClickEvent(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        g_con->injectMouseButtonDown(glfwToCeguiButton(button));
    }
    else if(action == GLFW_RELEASE)
    {
        g_con->injectMouseButtonUp(glfwToCeguiButton(button));
    }
    if(inputMouseClickCallback != nullptr)
        inputMouseClickCallback(window, button, action, mods);
}

void GUIHandler::mouseScrollEvent(GLFWwindow* window, double x, double y)
{
    g_con->injectMouseWheelChange(y);
    if(inputMouseScrollCallback != nullptr)
        inputMouseScrollCallback(window, x, y);
}

void GUIHandler::windowSizeChangeEvent(GLFWwindow* window, int w, int h)
{
    CEGUI::Size<float> newSize((float)w, (float)h);
    CEGUI::System::getSingleton().notifyDisplaySizeChanged(newSize);
    if(inputWindowSizeCallback != nullptr)
        inputWindowSizeCallback(window, w, h);
}
