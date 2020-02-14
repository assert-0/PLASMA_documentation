#pragma once

#include <cmath>

#define rad(x) (((x) / 360.0) * PI2)

//Constants

extern double PI;
extern double PI2;

//Base FOV
extern float g_baseFOV;
//The modifier is multiplied by the slider offset
//to control the range of values the slider can achieve.
extern float g_FOVModifier;

//Base mouse horizontal and vertical speeds along with
//their modifiers

extern double g_baseMouseSpeedH;
extern double g_baseMouseSpeedV;
extern double g_mouseSpeedHModifier;
extern double g_mouseSpeedVModifier;

//Base keyboard movement speed and its modifier

extern double g_baseKeyboardSpeed;
extern double g_keyboardSpeedModifier;

//Also known as Z-far. This value controls the distance
//at which objects will no longer render.
extern float g_maxRenderDistance;

//Radius multiplier between real particles and particle models.
//(radius/size scale)
extern double g_particleScale;

//Miscellaneous settings

//Screen resolution
extern int g_resolutionW, g_resolutionH;

//Camera position and rotation

extern int g_cameraPosX, g_cameraPosY, g_cameraPosZ;
extern double g_cameraAngleV, g_cameraAngleH;

//Background color
extern float g_backgroundR, g_backgroundG, g_backgroundB;

//GUI transparency
extern float g_guiAlpha;

//Cage model scale
extern float g_cageScale;
