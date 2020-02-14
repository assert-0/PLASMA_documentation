#include "settings.h"

//Constants

double PI = 3.14159265358979323846264338;
double PI2 = PI * 2.0;

//Base FOV
float g_baseFOV = rad(45.0);
//The modifier is multiplied by the slider offset
//to control the range of values the slider can achieve.
float g_FOVModifier = 1.0;

//Base mouse horizontal and vertical speeds along with
//their modifiers

double g_baseMouseSpeedH = 0.000025;
double g_baseMouseSpeedV = 0.000025;
double g_mouseSpeedHModifier = 5.0;
double g_mouseSpeedVModifier = 5.0;

//Base keyboard movement speed and its modifier

double g_baseKeyboardSpeed = 250.0;
double g_keyboardSpeedModifier = 2.0;

//Also known as Z-far. This value controls the distance
//at which objects will no longer render.
float g_maxRenderDistance = 10000000.0;

//Radius multiplier between real particles and particle models.
//(radius/size scale)
double g_particleScale = 5e8;

//Miscellaneous settings

//Screen resolution
int g_resolutionW = 1920 / 2, g_resolutionH = 1080 / 2;

//Camera position and rotation

int g_cameraPosX = -50, g_cameraPosY = -200, g_cameraPosZ = -500;
double g_cameraAngleV = rad(21.5), g_cameraAngleH = rad(5.7);

//Background color
float g_backgroundR = 0, g_backgroundG = 0, g_backgroundB = 0;

//GUI transparency
float g_guiAlpha = 0.9;

//Cage model scale
float g_cageScale = 0.1;
